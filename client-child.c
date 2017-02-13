#include "client.h"
#include <unistd.h>
#include "Pdu.h"

#define CLIENT_CHILD_LOG_FILE "child_log.txt"
#define WRITE_FILE_FAILED_RETRY_INTERVAL 5
#define SEND_TIMEOUT 5
#define RECV_TIMEOUT 8
#define MAX_ITEM_NUM_IN_A_PDU 30
#define RECV_REPLY_BUF 256
#define INVALID_ITEM_FILE_NAME "invalid_item.log"
#define FAILED_ITEM_POS_FILE_NAME "failed_item.log"
#define SET_SOCKOPT_FAILED_RETRY_INTERVAL 2
#define MALLOC_FAILED_RETRY_INTERVAL 5
#define READ_FAILED_BY_LOCK_RETRY_INTERVAL 2
#define OPEN_CONNECTION_TO_SERVER_RETRY_INTERVAL 5
#define READ_RESULT_BAK_BIN_FILE_RETRY_INTERVAL 4
#define OPEN_RESULT_BAK_BIN_FILE_RETRY_INTERVAL 5
#define OPEN_LAST_POS_MD5_FILE_RETRY_INTERVAL 4
#define WAIT_RESULT_BAK_BIN_FILE_CONTENT 5
#define OPEN_RESULT_BAK_BIN_FILE_LOCK_RETRY_INTERVAL 1
#define LAST_ITEM_POS_AND_MD5_ITEM_LEN 24 //8 bytes of uint64 of item count that has been processed in last result bak bin file from high bit to low bit, 16 bytes of md5 of content from 1 to previous item count of result bak bin file
#define LAST_RESULT_BAK_BIN_ITEM_POS_AND_MD5_FILE "last_result_bak_bin_item_pos_and_md5" //file content should be : 4 bytes of uint32 of item count that has been processed in last result bak bin file from high bit to low bit, 16 bytes of md5 of content from 1 to previous item count of result bak bin file

static int recv_reply_decode (int sockfd, void **asn_type, long type_len, asn_TYPE_descriptor_t *desc);
static int write_invalid_bytes_and_item_into_log (HANDLE fh, uint32_t num_bytes, char* item_buf);
static int retrieve_item_construct_monitor_data_item (unsigned char item_buf[RESULT_BAK_ITEM_BYTES], MonitorDataItem_t **mdi);
static int write_failed_item_pos_of_result_bak_bin_file_into_failed_log (HANDLE fh_failed_item, uint64_t pos, long item_count);
static int free_data_pdu (Pdu_t *pdu, uint32_t item_num);
static int open_socket (char *server, char *port);
static int process_result_bak_bin_item (unsigned char item_buf[], uint32_t item_count, long *item_nums, char *server, char *port);
static void check_arg (int argc, char *argv[]);
static int get_time_from_buf (unsigned char time_buf[8], long *year, long *month, long *day, long *hour, long *minute, long *second);
static int get_process_id (char *pid_string, DWORD *pid);
static int open_shared_memory (char *share_name, HANDLE *map_obj, DWORD map_obj_mode, LPVOID *map_addr, DWORD map_mode, uint64_t file_offset, SIZE_T mapping_size);
static int get_last_item_pos_and_md5 (HANDLE fh, unsigned char buf[LAST_ITEM_POS_AND_MD5_ITEM_LEN], uint64_t *pos);
static int calc_last_result_bak_bin_item_md5 (HANDLE fh_bak_bin, uint64_t pos, char md5[MD5LEN]);
static int get_integral_fraction_from_buf (unsigned char num_buf[8], long *integral, long *fraction);

static unsigned char item[RESULT_BAK_ITEM_BYTES * MAX_ITEM_NUM_IN_A_PDU];
static long pdu_number;
static HANDLE fh_last_md5; //FILE HANDLE of LAST_RESULT_BAK_BIN_ITEM_POS_AND_MD5_FILE
static unsigned char last_pos_md5[LAST_ITEM_POS_AND_MD5_ITEM_LEN];
static uint64_t last_item_pos;
static unsigned char *last_md5 = &last_pos_md5[8];
static e_site site;
static DWORD parent_process_id;
static LPVOID map_addr;
static HANDLE fh_bak_bin;
static char *map_name;
static HANDLE map_obj;
static FILE* fp_log;
static HANDLE fh_invalid_item, fh_failed_item;
HANDLE write_pipe, read_pipe; //two pipe to comminucate between two process

/*
 * argc1 is sitename, argc2 is file path of result ini file, argc3 is result backup file path, argc4 is result backup bin file path
 */
int main (int argc, char *argv[])
{
	int ret;
	LONG pos_low;
	LONG pos_high;
	HANDLE parent;
	PROCESS_INFORMATION pi;
	DWORD bytes;
	STARTUPINFO si;
	char *server_ip;
	char *port;

	if ((fp_log = freopen (CLIENT_CHILD_LOG_FILE, "a+", stdout)) == NULL) exit (2);
	if ((fp_log = freopen (CLIENT_CHILD_LOG_FILE, "a+", stderr)) == NULL) exit (2);
	if (open_file_handle (INVALID_ITEM_FILE_NAME, GENERIC_WRITE, 0, &fh_invalid_item))
	{
		fprintf (stderr, "open file of invalid item bytes failed. exit. line:%ld\n", __LINE__);
		exit (3);
	}
	if (SetFilePointer (fh_invalid_item, 0, NULL, FILE_END) ==  INVALID_SET_FILE_POINTER && GetLastError ())
	{
		fprintf (stderr, "set file pointer of invalid item bytes failed. exit. line:%ld\n", __LINE__);
		exit (3);
	}
	if (open_file_handle (FAILED_ITEM_POS_FILE_NAME, GENERIC_WRITE | GENERIC_READ, 0, &fh_failed_item))
	{
		fprintf (stderr, "open file of failed item bytes failed. exit. line:%ld\n", __LINE__);
		exit (3);
	}
	if (SetFilePointer (fh_failed_item, 0, NULL, FILE_END) ==  INVALID_SET_FILE_POINTER && GetLastError ())
	{
		fprintf (stderr, "set file pointer of failed item bytes failed. exit. line:%ld\n", __LINE__);
		exit (3);
	}
	//close (STDERR_FILENO);
	if (setvbuf (stdout, NULL, _IONBF, 0))
	{
		fprintf (stderr, "setvbuf for stdout failed. exit. err:%lu, line:%ld\n", GetLastError (), __LINE__);
		exit (1);
	}
	if (setvbuf (stderr, NULL, _IONBF, 0))
	{
		fprintf (stderr, "setvbuf for stderr failed. exit. err:%lu, line:%ld\n", GetLastError (), __LINE__);
		exit (1);
	}
	check_arg (argc, argv);
	//open shared memory
	if (open_shared_memory (map_name, &map_obj, MAP_OBJECT_MODE, &map_addr, MAP_OBJECT_MODE, 0, MAP_SIZE))
	{
		fprintf (stderr, "open_shared_memory failed. exit. line:%ld\n", __LINE__);
		exit (1);
	}
	//get read, write pipe from stdin, stdout
	if ((read_pipe = GetStdHandle (STD_INPUT_HANDLE)) == INVALID_HANDLE_VALUE)
	{
		fprintf (stderr, "GetStdHandle to get read pipe from stdin failed. exit. err:%lu, line:%ld\n", GetLastError (), __LINE__);
		exit (2);
	}
	if ((write_pipe = GetStdHandle (STD_OUTPUT_HANDLE)) == INVALID_HANDLE_VALUE)
	{
		fprintf (stderr, "GetStdHandle to get write pipe from stdin failed. exit. err:%lu, line:%ld\n", GetLastError (), __LINE__);
		exit (2);
	}
	//init socket library
	WSADATA wsaData;
	if (WSAStartup (MAKEWORD (1,1), &wsaData) != 0)
	{
		fprintf (stderr, "WSAStartup failed.\n");
		exit (1);
	}
	//open LAST_RESULT_BAK_BIN_ITEM_POS_AND_MD5_FILE file to get pos of last item that has been processed and md5 of content fo result bak bin file that has been processed
	while (open_file_handle (LAST_RESULT_BAK_BIN_ITEM_POS_AND_MD5_FILE, GENERIC_READ | GENERIC_WRITE, 0, &fh_last_md5))
	{
		fprintf (stderr, "open file of last item pos and md5 of result bak bin file failed. line:%ld\n", __LINE__);
		sleep (OPEN_LAST_POS_MD5_FILE_RETRY_INTERVAL);
	}
	if (get_last_item_pos_and_md5 (fh_last_md5, last_pos_md5, &last_item_pos)) last_item_pos = 0;
	else if (last_item_pos % RESULT_BAK_ITEM_BYTES)
	{
		printf ("last_item_pos:%" PRIu64 "\n", last_item_pos);
		last_item_pos = 0;
	}
	else
	{
		//open result bak bin file and check last md5
		while (open_file_handle (argv[2], GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, &fh_bak_bin))
		{
			fprintf (stderr, "open file of result bak bin file failed. line:%ld\n", __LINE__);
			sleep (OPEN_RESULT_BAK_BIN_FILE_RETRY_INTERVAL);
		}
		//calc md5 of item in open file handle
		char md5_from_bak_bin[MD5LEN];
		if (calc_last_result_bak_bin_item_md5 (fh_bak_bin, last_item_pos, md5_from_bak_bin)) last_item_pos = 0;
		else
		{
			for (int i = 0; i < MD5LEN; i++)
			{
				if (last_md5[i] != md5_from_bak_bin[i])
				{
					last_item_pos = 0;
					break;
				}
			}
		}
	}
	//open result bak bin file and wind to last pos
	while (1)
	{
		e_sign sign;
		pos_low = last_item_pos << 32 >> 32;
		pos_high = last_item_pos >> 32;
		long item_count;
		while (open_file_handle (argv[2], GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, &fh_bak_bin)) sleep (OPEN_RESULT_BAK_BIN_FILE_RETRY_INTERVAL) ;
		if (SetFilePointer (fh_bak_bin, pos_low, &pos_high, FILE_BEGIN) == INVALID_SET_FILE_POINTER && GetLastError ())
		{
			fprintf (stderr, "wind to last pos of result bak bin file failed. exit. err:%lu. line:%ld\n", GetLastError (), __LINE__);
			exit (2);
		}
		while (!ReadFile (fh_bak_bin, item, RESULT_BAK_ITEM_BYTES * MAX_ITEM_NUM_IN_A_PDU, &bytes, NULL))
		{
			if ((ret = GetLastError ()) == ERROR_LOCK_VIOLATION)
			{
				CloseHandle (fh_bak_bin);
				sleep (READ_FAILED_BY_LOCK_RETRY_INTERVAL);
				goto next;
			}
			else
			{
				fprintf (stderr, "ReadFile to read result file bak bin file err, exit, err:%lu, line:%ld\n", GetLastError (), __LINE__);
				CloseHandle (fh_bak_bin);
				sleep (READ_RESULT_BAK_BIN_FILE_RETRY_INTERVAL);
				goto next;
			}
		}
		if (bytes == 0)
		{
			CloseHandle (fh_bak_bin);
			sleep (WAIT_RESULT_BAK_BIN_FILE_CONTENT);
			continue;
		}
		if (bytes % RESULT_BAK_ITEM_BYTES)
		{
			CloseHandle (fh_bak_bin);
			fprintf (stderr, "ReadFile read bytes not multiple of RESULT_BAK_ITEM_BYTES");
			write_invalid_bytes_and_item_into_log (fh_invalid_item, bytes, item);
			sleep (WAIT_RESULT_BAK_BIN_FILE_CONTENT);
			continue;
		}
		//process item
		ret = process_result_bak_bin_item (item, bytes / RESULT_BAK_ITEM_BYTES, &item_count, argv[4], argv[5]);
		if (ret)
		{
			switch (ret)
			{
				case 1:
				fprintf (stderr, "asn_long2INTEGER to set some member of pdufailed.\n");
				break;
				case 2:
				fprintf (stderr, "asn_seqence_add to append item failed.\n");
				break;
				case 3:
				fprintf (stderr, "asn_long2INTEGER to set data item count failed.\n");
				break;
				case 4:
				fprintf (stderr, "encode data pdu failed\n");
				break;
			}
			fprintf (stderr, "asn_func failed and write failed item into log.\n");
			write_failed_item_pos_of_result_bak_bin_file_into_failed_log (fh_failed_item, last_item_pos, bytes / RESULT_BAK_ITEM_BYTES);
			sleep (WAIT_RESULT_BAK_BIN_FILE_CONTENT);
			continue;
		}
		else
		{
			//renew last item pos and md5
			last_item_pos += bytes;
			calc_file_md5 (item, RESULT_BAK_ITEM_BYTES, last_md5);
			//write into file
			while (SetFilePointer (fh_last_md5, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER && GetLastError ())
			{
				unsigned char num[8];
				num[0] = last_item_pos >> 56;
				num[1] = (last_item_pos << 8) >> 56;
				num[2] = (last_item_pos << 16) >> 56;
				num[3] = (last_item_pos << 24) >> 56;
				num[4] = (last_item_pos << 32) >> 56;
				num[5] = (last_item_pos << 40) >> 56;
				num[6] = (last_item_pos << 48) >> 56;
				num[7] = (last_item_pos << 56) >> 56;
				DWORD written;
				if (!WriteFile (fh_last_md5, num, 8, &written, NULL) || !WriteFile (fh_last_md5, last_md5, MD5LEN, &written, NULL))
				{
					fprintf (stderr, "write last file pos and md5 failed. last pos:%" PRIu64 ", md5:%s\n", last_item_pos, last_pos_md5);
				}
			}
			sleep (WAIT_RESULT_BAK_BIN_FILE_CONTENT);
			continue;
		}
next: ;
	}
}

static void check_arg (int argc, char *argv[])
{
	char *ch;
	long port;
	if (argc != 7)
	{
		fprintf (stderr, "child usage: progname sitename result_bak_bin_path, parent_process_id, server_addr, server_port. map_obj_name, given argc is %d. exit. line:%ld\n", argc, __LINE__);
		exit (1);
	}
	if ((site = get_asn_site_type_from_name (argv[1])) == site_invalid_site)
	{
		fprintf (stderr, "invalid site name. exit. line:%ld\n", __LINE__);
		exit (1);
	}
	if (check_path (argv[2]))
	{
		fprintf (stderr, "invalid result bak bin path. exit. line:%ld\n", __LINE__);
		exit (1);
	}
	if (get_process_id (argv[3], &parent_process_id))
	{
		fprintf (stderr, "invalid process id. exit. line:%ld\n", __LINE__);
		exit (1);
	}
	if (check_ip_addr (argv[4]))
	{
		fprintf (stderr, "invalid server ip addr. exit. line:%ld\n", __LINE__);
		exit (1);
	}
	if (check_port (argv[5]))
	{
		fprintf (stderr, "invalid port. exit. line:%ld\n", __LINE__);
		exit (1);
	}
	map_name = argv[6];
}

static int get_process_id (char *pid_string, DWORD *pid)
{
	char *ch;
	DWORD p;
	if (pid_string == NULL) return -1;
	for (p = 0, ch = pid_string; *ch != '\0'; ch++)
	{
		if (ch - pid_string > 9) return 1; //process id not more than 10 digit..
		if (!isdigit (*ch)) return 2;
		else p = p * 10 + *ch - '0';
	}
	return 0;
}

static int open_shared_memory (char *share_name, HANDLE *map_obj, DWORD map_obj_mode, LPVOID *map_addr, DWORD map_mode, uint64_t file_offset, SIZE_T mapping_size)
{
	if (share_name == NULL || map_obj == NULL || map_addr == NULL || mapping_size == 0) return -1;
	if ((*map_obj = OpenFileMapping (MAP_OBJECT_MODE, FALSE, map_name)) == NULL)
	{
		fprintf (stderr, "OpenFileMapping to open map object failed. err:%lu, line:%ld\n", GetLastError (), __LINE__);
		return 1;
	}
	if ((*map_addr = MapViewOfFile (*map_obj, map_mode, file_offset >> 32, (file_offset << 32) >> 32, mapping_size)) == NULL)
	{
		fprintf (stderr, "MapViewOfFile to map memory of object failed. err:%lu, line:%ld\n", GetLastError (), __LINE__);
		return 2;
	}
	return 0;
}

static int get_last_item_pos_and_md5 (HANDLE fh, unsigned char buf[LAST_ITEM_POS_AND_MD5_ITEM_LEN], uint64_t *pos)
{
	//not check arg
	DWORD bytes;
	uint64_t num;
	if (!ReadFile (fh, buf, LAST_ITEM_POS_AND_MD5_ITEM_LEN, &bytes, NULL))
	{
		fprintf (stderr, "readfile failed. err:%ld, line:%ld\n", GetLastError (), __LINE__);
		return 1;
	}
	if (bytes != LAST_ITEM_POS_AND_MD5_ITEM_LEN) return 2;
	num = (uint64_t)buf[0] << 56;
	num |= (uint64_t)buf[1] << 48;
	num |= (uint64_t)buf[2] << 40;
	num |= (uint64_t)buf[3] << 32;
	num |= (uint64_t)buf[4] << 24;
	num |= (uint64_t)buf[5] << 16;
	num |= (uint64_t)buf[6] << 8;
	num |= (uint64_t)buf[7];
	*pos = num;
	return 0;
}

static int calc_last_result_bak_bin_item_md5 (HANDLE fh_bak_bin, uint64_t pos, char md5[MD5LEN])
{
	DWORD bytes;
	DWORD ret;
	LONG high;
	char buf[RESULT_BAK_ITEM_BYTES];
	if (md5 == NULL) return -1;
	high = pos >> 32;
	if (SetFilePointer (fh_bak_bin, pos << 32 >> 32, &high, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{
		fprintf (stderr, "SetFilePointer to wind to last pos of result bin failed, err:%lu, line:%ld\n", ret, __LINE__);
		exit (2);
	}
	while (!ReadFile (fh_bak_bin, buf, RESULT_BAK_ITEM_BYTES, &bytes, NULL))
	{
		if ((ret = GetLastError ()) != ERROR_LOCK_VIOLATION)
		{
			fprintf (stderr, "ReadFile to read result bin failed, err:%lu, line:%ld\n", ret, __LINE__);
			exit (3);
		}
		sleep (OPEN_RESULT_BAK_BIN_FILE_LOCK_RETRY_INTERVAL);
		continue;
	}
	if (!CloseHandle (fh_bak_bin)) return 1;
	if (bytes < RESULT_BAK_ITEM_BYTES) return 2;
	if (calc_file_md5 (buf, bytes, md5))
	{
		fprintf (stderr, "calc_file_md5 of item in result bak bin file failed. line:%ld\n", __LINE__);
		return 3;
	}
	return 0;
}

static int get_time_from_buf (unsigned char time_buf[8], long *year, long *month, long *day, long *hour, long *minute, long *second)
{
	uint64_t time_num;
	unsigned char *ch;
	ch = (unsigned char*)time_buf;
	//here not check arg!!
	time_num = 0;
	time_num |= (uint64_t)ch[0] << 56;
	time_num |= (uint64_t)ch[1] << 48;
	time_num |= (uint64_t)ch[2] << 40;
	time_num |= (uint64_t)ch[3] << 32;
	time_num |= (uint64_t)ch[4] << 24;
	time_num |= (uint64_t)ch[5] << 16;
	time_num |= (uint64_t)ch[6] << 8;
	time_num |= (uint64_t)ch[7];
	*year = time_num / 10000000000;
	*month = (time_num % 10000000000) / 100000000;
	*day = (time_num % 100000000) / 1000000;
	if (check_year_month_day (*year, *month, *day)) return 1;
	*hour = (time_num % 1000000) / 10000;
	*minute = (time_num % 10000) / 100;
	*second = time_num % 100;
	if (*hour > 23 || *hour < 0 || *minute > 59 || *minute < 0 || *second > 61 || *second < 0) return 2;
	return 0;
}

static int get_integral_fraction_from_buf (unsigned char num_buf[8], long *integral, long *fraction)
{
	uint32_t integral_num, fraction_num;
	integral_num = num_buf[0] << 24;
	integral_num |= num_buf[1] << 16;
	integral_num |= num_buf[2] << 8;
	integral_num |= num_buf[3];
	fraction_num = num_buf[4] << 24;
	fraction_num |= num_buf[5] << 16;
	fraction_num |= num_buf[6] << 8;
	fraction_num |= num_buf[7];
	*integral = integral_num;
	*fraction = fraction_num;
	return 0;
}

static int process_result_bak_bin_item (unsigned char item_buf[], uint32_t item_count, long *item_nums, char *server, char *port)
{
	Pdu_t *data_pdu, *reply_pdu;
	int ret;
	char *data_enc;
	long data_enclen;
	long valid_num;
	MonitorDataItem_t *mdi;
	long pdu_no;
	//here not check arg
	valid_num = 0;

	while ((data_pdu = calloc (1, sizeof (Pdu_t))) == NULL)
	{
		fprintf (stderr, "calloc for data_pdu failed. line:%ld\n", __LINE__);
		sleep (MALLOC_FAILED_RETRY_INTERVAL);
	}
	//set some member of data pdu
	if (asn_long2INTEGER (&data_pdu->pdu_type, pdu_type_datapdu) || asn_long2INTEGER (&data_pdu->site, site))
	{
		free (data_pdu);
		write_invalid_bytes_and_item_into_log (fh_invalid_item, item_count * RESULT_BAK_ITEM_BYTES, item_buf);
		return 1;
	}
	data_pdu->pdu_packet.present = PduPacket_PR_monitor_data_packet;
	asn_sequence_empty (&data_pdu->pdu_packet.choice.monitor_data_packet.monitor_data_units.list); //empty monitor data units list
	//empty monitor data packet list
	//get and check each item from item buf
	for (uint32_t i = 0; i < item_count; i++)
	{
		if (retrieve_item_construct_monitor_data_item (item_buf, &mdi)) continue;
		//append to data_pdu
		if (asn_sequence_add (&data_pdu->pdu_packet.choice.monitor_data_packet.monitor_data_units.list, mdi))
		{
			//failed then free item and pdu
			free_data_pdu (data_pdu, valid_num + 1);
			return 2;
		}
		valid_num++;
	}
	if (valid_num == 0)
	{
		free (data_pdu);
		return 0;
	}
	pdu_no = pdu_number++;
	if (asn_long2INTEGER (&data_pdu->pdu_packet.choice.monitor_data_packet.monitor_data_unit_count, valid_num) || asn_long2INTEGER (&data_pdu->pdu_packet_number, pdu_no)) //set count of monitor item and pdu number of pdu
	{
		free_data_pdu (data_pdu, valid_num);
		return 3;
	}
	//xer_fprint (stdout, &asn_DEF_Pdu, data_pdu);

	//encode pdu
	if ((data_enc = encode_asn_type (data_pdu, &asn_DEF_Pdu, &data_enclen)) == NULL)
	{
		free_data_pdu (data_pdu, valid_num);
		return 4;
	}
	free_data_pdu (data_pdu, valid_num);

	//send encoded pdu while not get reply
	int sockfd;
	int send_timeout = SEND_TIMEOUT;
	int recv_timeout = RECV_TIMEOUT;
	long pdu_type, reply_site, reply;
	while (1)
	{
		if ((sockfd = open_socket (server, port)) == INVALID_SOCKET)
		{
			sleep (OPEN_CONNECTION_TO_SERVER_RETRY_INTERVAL);
			continue;
		}
		/*if (setsockopt (sockfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&send_timeout, sizeof (int)) == SOCKET_ERROR || setsockopt (sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&recv_timeout, sizeof (int)) == SOCKET_ERROR)
		{
			closesocket (sockfd);
			sleep (SET_SOCKOPT_FAILED_RETRY_INTERVAL);
			continue;
		}*/
		if (send (sockfd, data_enc, data_enclen, 0) == SOCKET_ERROR)
		{
			closesocket (sockfd);
			sleep (OPEN_CONNECTION_TO_SERVER_RETRY_INTERVAL);
			continue;
		}
		if (recv_reply_decode (sockfd, (void**)&reply_pdu, sizeof (Pdu_t), &asn_DEF_Pdu))
		{
			closesocket (sockfd);
			free (reply_pdu);
			sleep (OPEN_CONNECTION_TO_SERVER_RETRY_INTERVAL);
			continue;
		}
		long pdu_type, reply_site, reply, reply_no;
		if (asn_INTEGER2long (&reply_pdu->pdu_type, &pdu_type) || pdu_type != pdu_type_replypdu || asn_INTEGER2long (&reply_pdu->pdu_packet_number, &reply_no) || reply_no != pdu_no || asn_INTEGER2long (&reply_pdu->site, &reply_site) || reply_site != site || asn_INTEGER2long (&reply_pdu->pdu_packet.choice.monitor_data_reply, &reply) || reply != monitor_data_reply_success)
		{
			closesocket (sockfd);
			free (reply_pdu);
			sleep (OPEN_CONNECTION_TO_SERVER_RETRY_INTERVAL);
			continue;
		}
		free (data_enc);
		free (reply_pdu);
		break;
	}
	
	*item_nums = valid_num;
	return 0;
}

static int recv_reply_decode (int sockfd, void **asn_type, long type_len, asn_TYPE_descriptor_t *desc)
{
	char errbuf[128];
	size_t errlen = sizeof (errbuf);
	asn_dec_rval_t rval;
	int recv_bytes = 0;
	int consumed;
	int i = 0;
	char recv_buf[RECV_REPLY_BUF];
	
	//here not check arg
	
	while (1)
	{
		if((recv_bytes = recv (sockfd, recv_buf, RECV_REPLY_BUF, 0)) == SOCKET_ERROR || recv_bytes == 0)
		{
			if (i)
			{
				free (*asn_type);
				desc->free_struct (desc, *asn_type, 0);
			}
			return 1;
		}
		i++;
		for (consumed = 0; consumed < recv_bytes; )
		{
			rval = ber_decode (0, desc, (void **) asn_type, recv_buf + consumed, recv_bytes - consumed);
			if (rval.code == RC_OK)
			{
				if (asn_check_constraints (desc, *asn_type, errbuf, &errlen))
				{
					free (*asn_type);
					return 3;
				}
				else
				{
					return 0;
				}
			}
			else if (rval.code == RC_WMORE)
			{
				consumed += rval.consumed;
				continue;
			}
			else
			{
				desc->free_struct (desc, *asn_type, 0);
				free (*asn_type);
				return 2;
			}
		}
	}
}

static int open_socket (char *server, char *port)
{
	int sockfd, ret;
	struct addrinfo hints, *servinfo, *p;
	char s[INET6_ADDRSTRLEN];
	memset (&hints, 0, sizeof (hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	if (getaddrinfo (server, port, &hints, &servinfo)) return INVALID_SOCKET;
	for (p = servinfo; p != NULL; p = p->ai_next)
	{
		if ((sockfd = socket (p->ai_family, p->ai_socktype, p->ai_protocol)) == INVALID_SOCKET) continue;
		if (connect (sockfd, p->ai_addr, p->ai_addrlen) == SOCKET_ERROR)
		{
			closesocket (sockfd);
			continue;
		}
		break;
	}
	if (p == NULL)
	{
		freeaddrinfo (servinfo);
		return INVALID_SOCKET;
	}
	freeaddrinfo (servinfo);
	return sockfd;
}

static int free_data_pdu (Pdu_t *pdu, uint32_t item_num)
{
	for (uint32_t i = 0; i < item_num; i++) //free item
	{
		free (pdu->pdu_packet.choice.monitor_data_packet.monitor_data_units.list.array[i]);
	}
	free (pdu); //free data_pdu
	return 0;
}

static int retrieve_item_construct_monitor_data_item (unsigned char item_buf[RESULT_BAK_ITEM_BYTES], MonitorDataItem_t **mdi)
{
	long year, month, day, hour, minute, second, integral, fraction;
	e_site site_id;
	e_sign sign;
	e_type type;
	MonitorDataItem_t *m;

	if (check_type_enum (type = item_buf[0]) || get_time_from_buf (&item_buf[1], &year, &month, &day, &hour, &minute, &second) || ((sign = item_buf[9]) != sign_negative && sign != sign_positive) || (m = calloc (1, sizeof (MonitorDataItem_t))) == NULL)
	{
		fprintf (stderr, "item invalid. line:%ld\ncontent:%ld-%02ld-%02ld %ld:%02ld:%02ld %s %ld.%ld\n", __LINE__, year, month, day, hour, minute, second, get_type_name (type), integral, fraction);
		return 1;
	}
	get_integral_fraction_from_buf (&item_buf[10], &integral, &fraction);
	if (asn_long2INTEGER (&m->type, type) || asn_long2INTEGER (&m->monitor_unit, monitor_unit_micro_gram_per_m3) || asn_long2INTEGER (&m->monitor_value.sign, sign))
	{
		free (m);
		fprintf (stderr, "asn_long2INTEGER func to construct mdi failed. line:%ld\ncontent:%ld-%02ld-%02ld %ld:%02ld:%02ld %s %ld.%ld\n", __LINE__, year, month, day, hour, minute, second, get_type_name (type), integral, fraction);
		return 2;
	}
	m->datetime.year = year;
	m->datetime.month = month;
	m->datetime.day = day;
	m->datetime.hour = hour;
	m->datetime.minute = minute;
	m->datetime.second = second;
	m->monitor_value.integral = integral;
	m->monitor_value.fraction = fraction;
	*mdi = m;
	return 0;
}

static int write_invalid_bytes_and_item_into_log (HANDLE fh, uint32_t num_bytes, char* item_buf)
{
	char num[4];
	int ret;
	DWORD bytes;

	num[0] = num_bytes >> 24;
	num[1] = (num_bytes << 8) >> 24;
	num[2] = (num_bytes << 16) >> 24;
	num[3] = (num_bytes << 24) >> 24;
	//we first write 4 bytes of uint32 of length of item into invalid item log, and then write invalid item
	while (!WriteFile (fh, num, 4, &bytes, NULL))
	{
		if ((ret = GetLastError ()) == ERROR_DISK_FULL)
		{
			fprintf (stderr, "WriteFile to write invalid item failed. err:%lu, line:%ld\n", ret, __LINE__);
			return 1;
		}
		sleep (WRITE_FILE_FAILED_RETRY_INTERVAL);
	}
	while (!WriteFile (fh, item_buf, num_bytes, &bytes, NULL))
	{
		if ((ret = GetLastError ()) == ERROR_DISK_FULL)
		{
			SetFilePointer (fh, -4, NULL, FILE_CURRENT);
			fprintf (stderr, "WriteFile to write invalid item failed. err:%lu, line:%ld\n", ret, __LINE__);
			return 2;
		}
		sleep (WRITE_FILE_FAILED_RETRY_INTERVAL);
	}
	return 0;
}

static int write_failed_item_pos_of_result_bak_bin_file_into_failed_log (HANDLE fh_failed_item, uint64_t pos, long item_count)
{
	unsigned char num[8];
	int ret;
	DWORD bytes;
	//not check arg
	for (long i = 0; i < item_count; i++)
	{
		num[0] = pos >> 56;
		num[1] = (pos << 8) >> 56;
		num[2] = (pos << 16) >> 56;
		num[3] = (pos << 24) >> 56;
		num[4] = (pos << 32) >> 56;
		num[5] = (pos << 40) >> 56;
		num[6] = (pos << 48) >> 56;
		num[7] = (pos << 56) >> 56;
		while (!WriteFile (fh_failed_item, num, 8, &bytes, NULL))
		{
			if ((ret = GetLastError ()) == ERROR_DISK_FULL)
			{
				SetFilePointer (fh_failed_item, -4, NULL, FILE_CURRENT);
				fprintf (stderr, "WriteFile to write failed item failed. err:%lu, line:%ld\n", ret, __LINE__);
				return 2;
			}
			sleep (WRITE_FILE_FAILED_RETRY_INTERVAL);
		}
		pos += RESULT_BAK_ITEM_BYTES;
	}
}
