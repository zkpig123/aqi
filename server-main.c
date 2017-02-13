#include "server.h"

/*
 * item in data backup file(bin data):
 * siteid(1byte,enum) type(1byte,enum) time(8byte,uint64) sign(1byte,enum) integral(4byte,uint32) fraction(4byte,uint32)
 */

#define WRITE_BACKUP_FILE_MAX_TRY_TIMES 3
#define JOB_ITEM_SITE_BIT 0
#define JOB_ITEM_TYPE_BIT 1
#define JOB_ITEM_MONITOR_TIME_START_BIT 2
#define JOB_ITEM_INTEGRAL_START_BIT 11
#define JOB_ITEM_SIGN_BIT 10
#define JOB_ITEM_FRACTION_START_BIT 16
#define CHAR_BITS 8
#define WRITE_FILE_DISK_FULL_RETRY_INTERVAL 10
#define MAX_ITEM_NUM_IN_A_PDU 30
#define CHECK_THREAD_NUM_INTERVAL 1
#define MAX_DATA_PDU_ENCEDE_LEN 1024
#define MAIN_THREAD_NUM 1
#define ISSUE_ACCEPTEX_NUM 500
#define SHARE_FILE_NAME "server_share_file"
#define CALC_THREAD_NUM_FROM_PROCESSOR_NUM(num) (2 * num + 2)
#define WSASocket_RETRY_INTERVAL 5
#define BAK_FILE_ITEM_LEN 19
#define FILE_NAME_MAX 64
#define FILE_EXTENSION_NAME_MAX 12
#define PROCESS_ID_LEN_MAX 16 //HEX
#define ARG_BUF_LEN sizeof ("progname") - 1 + PATH_MAX + 1 + PROCESS_ID_LEN_MAX + 1 + IP_ADDR_MAX + 1 + 5 + 1 + sizeof (MAP_NAME) + 1 + 10 //5 is port len, 10 for protect
#define MONITOR_DATA_MYSQL_INSERT_QUERY_LEN sizeof ("UNLOCK TABLES;CREATE TABLE IF NOT EXISTS") + SITENAMEMAX + sizeof ("_")+ sizeof (RAW_TABLE_NAME_PREFIX)+ sizeof ("YYYYMMDD LIKE ")+ sizeof (RAW_TABLE_NAME_PREFIX)+ sizeof (";LOCK TABLES ")+ SITENAMEMAX + sizeof ("_")+ sizeof (RAW_TABLE_NAME_PREFIX)+ sizeof ("YYYYMMDD WRITE;INSERT IGNORE INTO ")+ SITENAMEMAX + sizeof ("_")+ sizeof (RAW_TABLE_NAME_PREFIX)+ sizeof ("YYYYMMDD (time,type,value,flag) VALUES ")+ sizeof ("('YYYY-MM-DD HH:MM:SS','")+ TYPE_NAME_LEN_MAX + sizeof ("',XXXXXXXXX.XXXXXXXXX,'1');")

void* worker_thread (void* arg);
static int process_io (PER_IO_CONTEXT *pIO, PER_SOCK_CONTEXT *ck, DWORD bytes_io);
void copy_item_into_buf (char *buf, long site, long type, DateTime_t *datetime, long sign, long integral, long fraction);
int decode_incoming (char *enc, uint64_t enclen, void **pdu, asn_TYPE_descriptor_t *desc);
int enqueue_job (char *data, long count, int site);
int issue_acceptex (SOCKET listen_sock);
int reuse_sockfd (PER_IO_CONTEXT *pIO);
struct job* retrieve_job (pthread_mutex_t *mutex);
int decode_datapdu (char *encode, DWORD bytes, Pdu_t **data_pdu, asn_TYPE_descriptor_t *desc);
static int share_memory (char *fn, HANDLE *fh, HANDLE *map_obj, char * map_name, uint64_t map_file_size, DWORD map_obj_mode, LPVOID *map_addr, DWORD map_mode, uint64_t file_offset, SIZE_T mapping_size);
static int create_process (char *arg, PROCESS_INFORMATION *pi, STARTUPINFO *si, HANDLE *write_pipe, HANDLE *read_pipe);
static void open_bak_file (HANDLE fh, char *fn1);
static void check_arg (int argc, char *argv[]);
int check_monitor_data_item (MonitorDataItem_t *mdi, DateTime_t *datetime,long *type, long *unit, long *sign, long *integral, long* fraction);
static int create_pipe (HANDLE *read_pipe, HANDLE *write_pipe);
static int get_file_pos (HANDLE fh, LONG *pos_low, PLONG pos_high);
static HANDLE comp_port;

struct job{
	char *data;
	long count;
	struct job *next;
} *job_queue;
pthread_mutex_t job_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t sync_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t thread_num_mutex;
sem_t job_sem;
pthread_cond_t sync_cond = PTHREAD_COND_INITIALIZER;
static HANDLE fh_bak, fh_share;
static int thread_num;
static exit_signal;
static SOCKET listen_sock;
static PER_SOCKET_CONTEXT *listen_sock_context;
static LPFN_ACCEPTEX lp_acceptex;
static LPFN_GETACCEPTEXSOCKADDRS lp_getacceptexsockaddrs;
static LPFN_DISCONNECTEX lp_disconnectex;

/*
 * argv1 is file path of data bak file, argv2 is bind addr, argv3 is bind port, argv4 is mysql server addr, argv5 is mysql server port
 */
int main (int argc, char *argv[])
{
	int ret;
	int first_run = 1;
	LONG pos_low;
	int tryCount;
	MYSQL *mysql = NULL;
	LONG pos_high;
	HANDLE write_pipe[2], read_pipe[2]; //two pipe to comminucate between two process
	HANDLE child;
	PROCESS_INFORMATION pi;
	DWORD cur_process_id;
	DWORD bytes;
	STARTUPINFO si;
	char arg[ARG_BUF_LEN];
	char result_buf[RESULT_FILE_BUF_LEN];
	struct job *job;
	char mysqlQuery[MONITOR_DATA_MYSQL_INSERT_QUERY_LEN * MAX_ITEM_NUM_IN_A_PDU + 1];
	unsigned long mysqlQueryLength;

	if (setvbuf (stdout, NULL, _IONBF, 0))
	{
		fprintf (stderr, "setvbuf for stdout failed. exit. err:%lu, line:%ld\n", GetLastError (), __LINE__);
		exit (1);
	}
	check_arg (argc, argv);
	open_bak_file (fh_bak, argv[1]); //open log file and set file pointer to eof
	get_file_pos (fh_bak, &pos_low, &pos_high); //get file pos of file end to lock result bak bin file which is shared between process
	lock_file_region (fh_bak, (DWORD)pos_low, (DWORD)pos_high, BAK_FILE_ITEM_LEN % UINT32_MAX, BAK_FILE_ITEM_LEN / UINT32_MAX); //lock 16 bytes from end of result backup file path
	//open file for share memory
	HANDLE map_obj;
	LPVOID map_addr;
	DWORD map_obj_mode = PAGE_READWRITE;
	if (share_memory (SHARE_FILE_NAME, &fh_share, &map_obj, MAP_NAME, MAP_SIZE, MAP_OBJECT_MODE, &map_addr, MAP_MODE, 0, MAP_SIZE))
	{
		fprintf (stderr, "share_memory failed. exit. line:%ld\n", __LINE__);
		exit (1);
	}
	//create pipe
	if (create_pipe (&write_pipe[0], &write_pipe[1]) || create_pipe (&read_pipe[0], &read_pipe[1]))
	{
		fprintf (stderr, "createpipe failed. exit. line:%ld\n", __LINE__);
		exit (1);
	}
	//get process id of current process
	cur_process_id = GetCurrentProcessId ();
	
	if (init_winsock_library ())
	{
		fprintf (stderr, "init winsock library failed. exit. err:%lu, line:%ld\n", GetLastError (), __LINE__);
		exit (1);
	}
	//init server(bind, listen)
	if (init_server_socket_stream (argv[2], argv[3], &listen_sock))
	{
		fprintf (stderr, "WSAStartup failed, exit.\n");
		exit (2);
	}
	//create completion port
	if ((comp_port = CreateIoCompletionPort (INVALID_HANDLE_VALUE, NULL, 0, 0)) == NULL)
	{
		fprintf (stderr, "create completion port failed. err:%lu. exit. line:%ld\n", GetLastError (), __LINE__);
		exit (3);
	}
	//create sock context for listen sock
	if ((listen_sock_context = calloc (1, sizeof (PER_SOCKET_CONTEXT))) == NULL)
	{
		fprintf (stderr, "create sock context for listen sock failed. exit. line:%ld\n", __LINE__);
		exit (3);
	}
	//bind listen sock to completion port
	if ((comp_port = CreateIoCompletionPort ((HANDLE)listen_sock, comp_port, listen_sock_context, 0)) == NULL)
	{
		fprintf (stderr, "bind listen sock to completion port failed. err:%lu. exit. line:%ld\n", GetLastError (), __LINE__);
		exit (3);
	}
	//get func pointer of acceptex and getacceptexsockaddrs
	if ((lp_acceptex = get_GetAcceptExSockAddrs_func_pointer (listen_sock)) == NULL || (lp_getacceptexsockaddrs = get_AcceptEx_func_pointer (listen_sock)) == NULL || (lp_disconnectex = get_DisconnectEx_func_pointer (listen_sock)) == NULL)
	{
		fprintf (stderr, "get pointer of func acceptex and getacceptexsockaddrs failed. exit. line:%ld\n", __LINE__);
		exit (3);
	}

	//pass arg : progname, data_bak_file_path, processid_of_parent, mysql_ip_addr, mysql_port, map_name
	if ((ret = snprintf (arg, ARG_BUF_LEN, "%s %s %s %lu %s %s %s", "progname", argv[1], cur_process_id, argv[4], argv[5], MAP_NAME)) < 0 || ret + 1 > ARG_BUF_LEN)
	{
		fprintf (stderr, "buffer for child arg is not enough. exit. err:%lu, line:%ld\n", GetLastError (), __LINE__);
		exit (1);
	}
	//create process
	create_process (arg, &pi, &si, write_pipe, read_pipe);

	//init semaphore for job queue
	if (sem_init (&job_sem, 0, 0) == -1)
	{
		fprintf (stderr, "init semaphore for job queue failed.\n");
		exit (1);
	}

	//create work thread
	DWORD thread_num, processor_num;
	get_processor_num (&processor_num);
	thread_num = CALC_THREAD_NUM_FROM_PROCESSOR_NUM(processor_num);
	pthread_t pthread_id;
	pthread_attr_t attr;
	//set attribute of work thread to be detach so we don't need to process threads having exited
	if (pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED))
	{
		fprintf (stderr, "set attr of pthread failed. exit. errno:%ld, line:%ld\n", __LINE__);
		exit (2);
	}
	//create work thread
	for (int i = 0; i < thread_num - 1; i++)
	{
		if (pthread_create (&pthread_id, &attr, worker_thread, (void*) comp_port))
		{
			fprintf (stderr, "pthread_create failed. exit. errno:%ld, line:%ld\n", errno, __LINE__);
			exit (2);
		}
	}

	//get mysql conn
	while (1)
	{
		if ((mysql = get_mysql_connection_multi_statement ()) != NULL) break;
	}

	//issue acceptex to accept incoming connection
	for (int i = 0; i < ISSUE_ACCEPTEX_NUM; i++)
	{
		if (issue_acceptex (listen_sock)) fprintf (stderr, "issue acceptex failed.\n");
	}


	while (1)
	{
		//check and create child process
		if ((child = OpenProcess (PROCESS_QUERY_INFORMATION, FALSE, pi.dwProcessId)) == NULL) create_process (arg, &pi, &si, write_pipe, read_pipe);
		else CloseHandle (child);

		if (first_run)
		{
			while (1)
			{
				int tmp;
				pthread_mutex_lock (&thread_num_mutex);
				tmp = thread_num;
				pthread_mutex_unlock (&thread_num_mutex);
				if (tmp) sleep (CHECK_THREAD_NUM_INTERVAL);
				else break;
			}
			pthread_mutex_lock (&sync_mutex);
			pthread_cond_broadcast (&sync_cond);
			pthread_mutex_unlock (&sync_mutex);
			first_run = 0;
		}

		//wait for job
		sem_wait (&job_sem);
		//retrieve job
		if ((job = retrieve_job (&job_mutex)) == NULL) continue;
		//get current pos of backup file handle
		pos_low = get_file_pos (fh_bak, &pos_low, &pos_high);
		//lock file region to write
		while (!LockFile (fh_bak, pos_low, pos_high, BAK_FILE_ITEM_LEN * job->count % UINT32_MAX, BAK_FILE_ITEM_LEN * job->count / UINT32_MAX))
		{
			if ((ret = GetLastError ()) != ERROR_LOCK_VIOLATION) fprintf (stderr, "LockFile failed. err:%lu, line:%ld\n", ret, __LINE__);
		}
		tryCount = WRITE_BACKUP_FILE_MAX_TRY_TIMES;
		//write to bak file
		while (!WriteFile (fh_bak, job->data, job->count * BAK_FILE_ITEM_LEN, &bytes, NULL))
		{
			fprintf (stderr, "WriteFile failed!!. err:%lu, line:%ld\n", GetLastError (), __LINE__);
			if (--tryCount < 0) break;
		}
		//unlock file region
		while (!UnlockFile (fh_bak, pos_low, pos_high, BAK_FILE_ITEM_LEN * job->count % UINT32_MAX, BAK_FILE_ITEM_LEN * job->count / UINT32_MAX))
		{
			fprintf (stderr, "UnlockFile failed!!. err:%lu, line:%ld\n", ret, __LINE__);
		}
		//try to import into mysql file not more than twice
		mysqlQueryLength = 0;
		int cur_job_index, cur_query_buf_index;
		for (cur_job_index = 0, cur_query_buf_index = 0; cur_job_index < job->count; cur_job_index++)
		{
			uint64_t monitorTime = 0;
			char *job_current_data, sitename;
			uint32_t integral_unsigned = 0, fraction = 0;
			long sign, integral, year, month, day, hour, minute, second;
			job_current_data = job->data + i * BAK_FILE_ITEM_LEN;
			sitename = get_site_name (job_current_data[JOB_ITEM_SITE_BIT]);
			for (int i = 7; i >= 0; i++) monitorTime |= job_current_data[i + JOB_ITEM_MONITOR_TIME_START_BIT] << (i * CHAR_BITS);
			year = monitorTime / 10000000000;
			month = (monitorTime % 10000000000) / 100000000;
			day = (monitorTime / 100000000) % 1000000;
			hour = (monitorTime % 1000000) / 10000;
			minute = (monitorTime % 10000) / 100;
			second = monitorTime % 100;
			for (int i = 3; i >= 0; i++) integral_unsigned |= job_current_data[i + JOB_ITEM_INTEGRAL_START_BIT] << (i * CHAR_BITS); 
			integral = (sign == sign_negative ? -integral_unsigned : integral_unsigned);
			for (int i = 3; i >= 0; i++) fraction |= job_current_data[i + JOB_ITEM_FRACTION_START_BIT] << (i * CHAR_BITS); 
			if ((ret = snprintf (mysqlQuery + MONITOR_DATA_MYSQL_INSERT_QUERY_LEN * i, MONITOR_DATA_MYSQL_INSERT_QUERY_LEN, "UNLOCK TABLES;CREATE TABLE IF NOT EXISTS %s_%s%ld%02ld%02ld LIKE %s;LOCK TABLES %s_%s%ld%02ld%02ld WRITE;INSERT IGNORE INTO %s_%s%ld%02ld%02ld (time,type,value,flag) VALUES ('%ld-%02ld-%02ld %ld:%02ld:%02ld','%s',%ld.%ld,'1');", sitename, RAW_TABLE_NAME_PREFIX, year, month, day, RAW_TABLE_TEMPLATE, sitename, RAW_TABLE_NAME_PREFIX, year, month, day, sitename, RAW_TABLE_NAME_PREFIX, year, month, day, year, month, day, hour, minute, second, get_type_name (job_current_data[JOB_ITEM_TYPE_BIT]), integral, fraction)) < 0 || ret + 1 > MONITOR_DATA_MYSQL_INSERT_QUERY_LEN + 1)
			{
				fprintf (stderr, "buffer for mysqlQuery is not enough. line:%ld\n", __LINE__);
				exit (2); //fatal error
			}
			else
			{
				mysqlQueryLength += ret;
				cur_query_buf_index++;
			}
		}
		unsigned my_errno;
		MYSQL_RES *res;
		int status;
		tryCount = 2;
		while (1)
		{
			if ((ret = mysql_real_query (mysql, mysqlQuery, mysqlQueryLength)))
			{
				if ((my_errno = mysql_errno (mysql)) == CR_SERVER_GONE_ERROR || ret == CR_SERVER_LOST)
				{
					mysql_close (mysql);
					if ((mysql = get_mysql_connection_multi_statement ()) == NULL)
					{
						fprintf (stderr, "get mysql conn failed. line:%ld\n", __LINE__);
						break;
					}
					else continue;
				}
				mysql = NULL;
				fprintf (stderr, "mysql_real_query failed. mysql errno:%u, mysql error:%s, line:%ld\n", my_errno, mysql_error (mysql), __LINE__);
				break;
			}
			if (--tryCount < 0) break;
		}
		if (!ret) //mysql_real_query success
		{
			do
			{
				res = (mysql_store_result (mysql));
				if (res) mysql_free_result;
				if ((status = mysql_next_result (mysql)) == 0) continue;
				else if (status == -1) break;
				else
				{
					fprintf (stderr, "%s, mysql_next_result func err. line:%ld\n", __LINE__);
				}
			} while (status == 0);
		}
		//free job
		free (job->data);
		free (job);
next: ;

	}
STOP_SERVER:;
	closesocket (listen_sock);
	WSACleanup ();
}

static int create_process (char *arg, PROCESS_INFORMATION *pi, STARTUPINFO *si, HANDLE *write_pipe, HANDLE *read_pipe)
{
	if (arg == NULL || pi == NULL || si == NULL || write_pipe == NULL || read_pipe == NULL) return -1;
	memset (si, 0, sizeof (STARTUPINFO));
	memset (pi, 0, sizeof (STARTUPINFO));
	si->cb = sizeof (STARTUPINFO);
	//si->wShowWindow = SW_HIDE;
	si->dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	si->hStdInput = write_pipe[0];
	si->hStdOutput = read_pipe[1];
	/*if (!CreateProcess (CHILD_PROCESS_NAME, arg, NULL, NULL, TRUE, 0, NULL, NULL, si, pi))
	{
		fprintf (stderr, "createprocess failed. err:%lu, line:%ld\n", GetLastError (), __LINE__);
		return 1;
	}*/
	return 0;
}

static int create_pipe (HANDLE *read_pipe, HANDLE *write_pipe)
{
	if (read_pipe == NULL || write_pipe == NULL) return -1;
	SECURITY_ATTRIBUTES sa;
	memset (&sa, 0, sizeof (SECURITY_ATTRIBUTES));
	sa.nLength = sizeof (SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = NULL;
	if (!CreatePipe (read_pipe, write_pipe, &sa, 0))
	{
		fprintf (stderr, "createpipe failed, exit. err:%ld, line:%ld\n", GetLastError (), __LINE__);
		exit (1);
	}
	/*if (!SetHandleInformation (read_pipe[0], HANDLE_FLAG_INHERIT, 0) || !SetHandleInformation (write_pipe[1], HANDLE_FLAG_INHERIT, 0)) //Ensure some pipe is not inherited by child
	{
		fprintf (stderr, "SetHandleInformation failed, exit. err:%ld, line:%ld\n", GetLastError (), __LINE__);
		exit (1);
	}
	if (!SetHandleInformation (read_pipe[0], HANDLE_FLAG_INHERIT, 0) || !SetHandleInformation (write_pipe[1], HANDLE_FLAG_INHERIT, 0)) return 1; //Ensure some pipe is not inherited by child*/
	return 0;
}

static void open_bak_file (HANDLE fh, char *fn)
{
	if (open_file_handle (fn, GENERIC_WRITE, FILE_SHARE_READ, &fh))
	{
		fprintf (stderr, "open result bak file failed. line:%ld\n", __LINE__);
		exit (3);
	}
	if (SetFilePointer (fh, 0, NULL, FILE_END) == INVALID_SET_FILE_POINTER)
	{
		fprintf (stderr, "wind to end of result bak file failed. exit. err:%lu. line:%ld\n", GetLastError (), __LINE__);
		exit (3);
	}
}

static void check_arg (int argc, char *argv[])
{
	char *ch;
	long port;
	if (argc != 4)
	{
		fprintf (stderr, "usage: progname sitename data_bak_file_path bind_addr bind-port. given argc is %d. exit. line:%ld\n", argc, __LINE__);
		exit (1);
	}
	if (check_path (argv[1]))
	{
		fprintf (stderr, "invalid data bak file path. exit. line:%ld\n", __LINE__);
		exit (1);
	}
	if (check_ip_addr (argv[2]))
	{
		fprintf (stderr, "invalid bind addr. exit. line:%ld\n", __LINE__);
		exit (1);
	}
	if (check_port (argv[3]))
	{
		fprintf (stderr, "invalid port. exit. line:%ld\n", __LINE__);
		exit (1);
	}
	if (check_ip_addr (argv[4]))
	{
		fprintf (stderr, "invalid mysql addr. exit. line:%ld\n", __LINE__);
		exit (1);
	}
	if (check_port (argv[5]))
	{
		fprintf (stderr, "invalid mysql port. exit. line:%ld\n", __LINE__);
		exit (1);
	}
}

static int share_memory (char *fn, HANDLE *fh, HANDLE *map_obj, char * map_name, uint64_t map_file_size, DWORD map_obj_mode, LPVOID *map_addr, DWORD map_mode, uint64_t file_offset, SIZE_T mapping_size)
{
	if (fn == NULL || fh == NULL || map_obj == NULL || map_name == NULL || map_addr == NULL) return -1;
	if (open_file_handle (fn, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, fh))
	{
		fprintf (stderr, "open share memory file failed. line:%ld\n", __LINE__);
		return 1;
	}
	if ((*map_obj = CreateFileMapping (*fh, NULL, map_obj_mode, map_file_size >> 32, (map_file_size << 32) >> 32, map_name)) == NULL)
	{
		fprintf (stderr, "CreateFileMapping to create map object failed. err:%lu, line:%ld\n", GetLastError (), __LINE__);
		return 2;
	}
	if ((*map_addr = MapViewOfFile (*map_obj, map_mode, file_offset >> 32, (file_offset << 32) >> 32, mapping_size)) == NULL)
	{
		fprintf (stderr, "MapViewOfFile to map memory of object failed. err:%lu, line:%ld\n", GetLastError (), __LINE__);
		return 3;
	}
	return 0;
}

static int get_file_pos (HANDLE fh, LONG *pos_low, PLONG pos_high)
{
	LONG low;
	LONG high;
	if (fh == INVALID_HANDLE_VALUE || pos_low == NULL || pos_high == NULL) return -1;
	high = 0;
	if ((low = SetFilePointer (fh, 0, &high, FILE_CURRENT)) == INVALID_SET_FILE_POINTER && GetLastError())
	{
		fprintf (stderr, "setfilepointer to get cur pos failed, exit. err:%lu. line:%ld\n", GetLastError (), __LINE__);
		exit (3);
	}
	*pos_low = low;
	*pos_high = high;
	return 0;
}

int issue_acceptex (SOCKET listen_sock)
{
	int ret;
	DWORD err;
	DWORD bytes;
	PER_IO_CONTEXT *pIO;
	pIO = calloc (1, sizeof (PER_IO_CONTEXT));
	if (pIO == NULL) return 1;
	pIO->wsabuf.buf = pIO->buf;
	pIO->wsabuf.len = MAX_DATA_PDU_ENCEDE_LEN;
	pIO->optype = op_accept;
	pIO->sock_context = NULL;
	while ((pIO->sock = WSASocket (AF_UNSPEC, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
	{
		fprintf (stderr, "WSASocket failed. err:%lu, line:%ld\n", GetLastError (), __LINE__);
		sleep (WSASocket_RETRY_INTERVAL);
	}
	ret = lp_acceptex (listen_sock, pIO->sock, pIO->buf, sizeof (pIO->buf - 2 * (sizeof struct sockaddr + 16)), sizeof struct sockaddr, sizeof struct sockaddr, &bytes, &pIO->overlapped);
	if (ret == FALSE)
	{
		if ((err = WSAGetLastError ()) != ERROR_IO_PENDING)
		{
			fprintf (stderr, "acceptex failed. error:%lu, line:%ld\n", WSAGetLastError(), err, __LINE__);
			return 2;
		}
		else return 0;
	}
	return 0;
}

void* worker_thread (void* arg)
{
	HANDLE cp = (HANDLE) arg;
	DWORD io_size;
	PER_SOCKET_CONTEXT *ck;
	PER_IO_CONTEXT *pIO;
	DWORD bytes_io;
	int ret;
	DWORD err;

	pthread_mutex_lock (&thread_num_mutex);
	thread_num--;
	pthread_mutex_unlock (&thread_num_mutex);
	//wait for start signal
	pthread_mutex_lock (&sync_mutex);
	pthread_cond_wait (&sync_cond, &sync_mutex);
	pthread_mutex_unlock (&sync_mutex);

	while (!exit_signal)
	{
		pIO = NULL;
		ret = GetQueueCompletionStatus (comp_port, &bytes_io, &ck, (LPOVERLAPPED *) &pIO, INFINITE);
		if (!ret)
		{
			err = GetLastError ();
			if (err == WAIT_TIMEOUT)
			{
				if (send (ck->sock, "", 0, 0) == SOCKET_ERROR) //check client alive
				{
					//client abort
					//recycle socket and issue new acceptex
					if (reuse_sockfd (pIO))
					{
						fprintf (stderr, "client abort and reuse sockfd failed. line:%ld\n", __LINE__);
						free (pIO);
						if (pIO->sock_context != NULL && pIO->sock_context != listen_sock_context) free (pIO->sock_context);
						closesocket (pIO->sockfd);
						continue;
					}
				}
				continue; //timeout, retry
			}
			else (err == ERROR_NETNAME_DELETED) //client abort
			{
				//recyle socket and issue new acceptex
				if (reuse_sockfd (pIO))
				{
					fprintf (stderr, "client abort and reuse sockfd failed. line:%ld\n", __LINE__);
					free (pIO);
					if (pIO->sock_context != NULL && pIO->sock_context != lisen_sock_context) free (pIO->sock_context);
					closesocket (pIO->sockfd);
					continue;
				}
			}
			else //completion port err! exit!!
			{
				CancelIo ((HANDLE)ck->sock); //cancel io of this sock
				closesocket (ck->sock);
				if (pIO != NULL)
				{
					free (pIO);
					if (pIO->sock_context != NULL && pIO->sock_context != listen_sock_context) free (pIO->sock_context);
				}
				fprintf (stderr, "completion port error. thread exit!. line:%ld\n", __LINE__);
				exit_signal = 1;
				break;
			}
		}
		else
		{
			if (bytes_io == 0 && (pIO->optype == op_send || pIO->optype == op_recv)) //client disconnect
			{
				//recycle sock and resource
				if (reuse_sockfd (pIO))
				{
					fprintf (stderr, "client abort and reuse sockfd failed. line:%ld\n", __LINE__);
					free (pIO);
					if (pIO->sock_context != NULL && pIO->sock_context != lisen_sock_context) free (pIO->sock_context);
					closesocket (pIO->sockfd);
					continue;
				}
				continue;
			}
			else process_io (pIO, ck, bytes_io);
		}
	}
}

static int process_io (PER_IO_CONTEXT *pIO, PER_SOCK_CONTEXT *ck, DWORD bytes_io)
{
	struct sockaddr *local_addr, *remote_addr;
	int local_addrlen, remote_addrlen;
	local_addr = NULL;
	remote_addr = NULL;
	local_addrlen = sizeof (struct sockaddr);
	DWORD bytes;
	remote_addrlen = sizeof (struct sockaddr);
	switch (pIO->optype)
	{
		case op_reuse:
		//reuse sockfd
		memset (&pIO->overlapped, 0, sizeof (OVERLAPPED));
		lp_acceptex (listen_sock, pIO->sock, pIO->buf, sizeof (pIO->buf - 2 * (sizeof struct sockaddr + 16)), sizeof struct sockaddr, sizeof struct sockaddr, &bytes, &pIO->overlapped);
		return 0;

		case op_read:
		//we don't need op_read, if got here, reuse it
		fprintf (stderr, "got WSARecv?? it's impossible, we don't arrange it. line:%ld\n", __LINE__);
		return 4;

		case op_send:
		if (reuse_sockfd (pIO))
		{
			free (pIO);
			free (pIO->sock_context);
			return 5;
		}
		return 0;

		case op_accept:
		//get data and decode data pdu
		setsockopt (pIO->sock, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (char*)&(ck->sock), sizeof(pComKey->sock)); 
		lp_getacceptexsockaddrs (pIO->buf, sizeof (pIO->buf - 2 * (sizeof struct sockaddr + 16)), sizeof (struct sockaddr), sizeof (struct sockaddr), (LPSOCKADDR*)&local_addr, &local_addrlen, (LPSOCKADDR*)&remote_addr, &remote_addrlen);
		//decode
		Pdu_t *data_pdu;
		long pdu_type, pdu_no, site, item_count;
		char *new_data;
		//xer_fprint (stdout, &asn_DEF_Pdu, *data_pdu);
		if (decode_datapdu (pIO->buf, bytes_io, &data_pdu, &asn_DEF_Pdu))
		{
			//decode failed then reuse
			if (reuse_sockfd (pIO))
			{
				free (pIO);
				if (pIO->sock_context != NULL) free (pIO->sock_context);
				return 5;
			}
			return 0;
		}
		DateTime_t datetime;
		long type, monitor_unit, integral, fraction, sign, valid_count;
		valid_count = 0;
		for (long int i = 0; i < monitor_unit; i++)
		{
			char *cur;
			uint64_t dt;
			uint32_t num;
			if (check_monitor_data_item (data_pdu->pdu_packet.choice.monitor_data_packet.monitor_data_units.list.array[i], &datetime, &type, &unit, &sign, &integral, &fraction)) continue;
			else
			{
				cur = new_data + valid_count * BAK_FILE_ITEM_LEN;
				copy_item_into_buf (cur, site, type, &datetime, sign, integral, fractoin);
				valid_count++;
			}
		}
		free (data_pdu);
		if (valid_count == 0)
		{
			free (data_pdu);
			if (reuse_sockfd (pIO))
			{
				free (pIO);
				if (pIO->sock_context != NULL) free (pIO->sock_context);
				return 5;
			}
			return 0;
		}
		//construct reply pdu and encode
		Pdu_t *reply_pdu;
		char *reply_enc;
		long reply_enclen;
		if ((reply_pdu = calloc (1, sizeof (Pdu_t))) == NULL)
		{
			if (reuse_sockfd (pIO))
			{
				free (pIO);
				if (pIO->sock_context != NULL) free (pIO->sock_context);
				return 5;
			}
		}
		if (asn_long2INTEGER (&reply_pdu->pdu_type, pdu_type_replypdu) || asn_long2INTEGER (&reply_pdu->pdu_packet_number, pdu_no) || asn_long2INTEGER (&reply_pdu->site, site) || asn_long2INTEGER (&reply_pdu->pdu_packet.choice.monitor_data_reply, monitor_data_reply_success))
		{
			free (reply_pdu);
			if (reuse_sockfd (pIO))
			{
				free (pIO);
				if (pIO->sock_context != NULL) free (pIO->sock_context);
				return 5;
			}
		}
		reply_pdu->pdu_packet.present = PduPacket_PR_monitor_data_reply;
		if ((reply_enc = encode_asn_type (reply_pdu, &asn_DEF_Pdu, &reply_enclen)) == NULL)
		{
			free (reply_pdu);
			if (reuse_sockfd (pIO))
			{
				free (pIO);
				if (pIO->sock_context != NULL) free (pIO->sock_context);
				return 5;
			}
		}
		else
		{
			free (reply_pdu);
			//bind new sock to completion port
			if (pIO->sock_context == NULL) //create sock context if accepted sock is not a reuse sock which has already a sock context
			{
				if ((pIO->sock_context = calloc (1, sizeof (PER_SOCKET_CONTEXT))) == NULL)
				{
					free (reply_enc);
					return 1;
				}
			}
			pIO->sock_context.sock = pIO->sock;
			if (CreateIoCompletionPort (pIO->sock, comp_port, (DWORD)new_ck, 0) == NULL)
			{
				free (reply_enc);
				fprintf (stderr, "bind new sock to completion port failed. err:%lu, line:%ld\n", GetLastError (), __LINE__);
				if (reuse_sockfd (pIO))
				{
					free (pIO);
					free (pIO->sock_context);
					return 5;
				}
				return 3;
			}
			memset (&pIO->overlapped, 0, sizeof (OVERLAPPED));
			pIO->type = op_send;
			memcpy (pIO->wsabuf.buf, reply_enc, reply_enclen);
			DWORD recv_bytes;
			if (WSASend (pIO->sock_context->sock, &pIO->wsabuf, 1, &reply_enclen, &pIO->overlapped, NULL) == SOCKET_ERROR)
			{
				if ((err = WSAGetLastError ()) != ERROR_IO_PENDING)
				{
					fprintf (stderr, "issue WSAsend io failed. err:%lu, line:%ld\n", err, __LINE__);
				}
			}
			free (reply_enc);
			if (reuse_sockfd (pIO))
			{
				free (pIO);
				free (pIO->sock_context);
				return 5;
			}
		}
		
		//enqueue job
		if (enqueue_job (new_data, valid_count, site))
		{
			fprintf (stderr, "malloc for job failed. line:%ld\n", __LINE__);
			////////////////////write to per-thread err item log
		}
		if (reuse_sockfd (pIO))
		{
			free (pIO);
			if (pIO->sock_context != NULL) free (pIO->sock_context);
			return 5;
		}
	}
}


void copy_item_into_buf (char *buf, long site, long type, DateTime_t *datetime, long sign, long integral, long fraction)
{
	uint64_t dt;
	uint32_t num;
	dt = datetime->year * 10000000000 + datetime->month * 100000000 + datetime->day * 1000000 + datetime->hour * 10000 + datetime->hour * 100 + datetime->second;
	buf[0] = site;
	buf[1] = type;
	buf[2] = dt >> 56;
	buf[3] = dt << 8 >> 56;
	buf[4] = dt << 16 >> 56;
	buf[5] = dt << 24 >> 56;
	buf[6] = dt << 32 >> 56;
	buf[7] = dt << 40 >> 56;
	buf[8] = dt << 48 >> 56;
	buf[9] = dt << 56 >> 56;
	buf[10] = sign;
	num = integral;
	buf[11] = num >> 24;
	buf[12] = num << 8 >> 24;
	buf[13] = num << 16 >> 24;
	buf[14] = num << 24 >> 24;
	num = fraction;
	buf[15] = num >> 24;
	buf[16] = num << 8 >> 24;
	buf[17] = num << 16 >> 24;
	buf[18] = num << 24 >> 24;
}

int decode_incoming (char *enc, uint64_t enclen, void **pdu, asn_TYPE_descriptor_t *desc)
{
	if (enc == NULL || enclen <= 0 || pdu == NULL || desc == NULL) return -1;
	char *cur;
	char errbuf[128];
	size_t errlen = sizeof (errbuf);
	asn_dec_rval_t rval;
	uint64_t consumed;
	int i = 0;

	for (consumed = 0, cur = enc; consumed < enclen; )
	{
		rval = ber_decode (0, desc, pdu, cur + consumed, enclen - consumed);
		if (rval.code == RC_OK)
		{
			if (asn_check_constraints (desc, *pdu, errbuf, &errlen))
			{
				free (*pdu);
				return 1;
			}
			else return 0;
		}
		else if (rval.code == RC_WMORE)
		{
			consumed += rval.consumed;
			continue;
		}
		else
		{
			desc->free_struct (desc, *pdu, 0);
			free (*pdu);
			return 2;
		}
	}
	if (rval.code != RC_OK) return 3;
}

int check_monitor_data_item (MonitorDataItem_t *mdi, DateTime_t *datetime,long *type, long *unit, long *sign, long *integral, long* fraction)
{
	//not check arg
	if (check_year_month_day (mdi->datetime.year, mdi->datetime.month, mdi->datetime.day)) return 1;
	if (mdi->datetime.hour < 0 || mdi->datetime.hour > 23 || mdi->datetime.minute < 0 || mdi->datetime.minute > 59 || mdi->datetime.second < 0 || mdi->datetime.second > 60) return 2;
	if (asn_INTEGER2long (&mdi->type, type) || *type < type_so2 || *type > type_doas_adjust_poll_pos || asn_INTEGER2long (&mdi->monitor_unit, unit) || (*unit != monitor_unit_milli_gram_per_m3 && *unit != monitor_unit_micro_gram_per_m3) || asn_INTEGER2long (&mdi->monitor_value.sign, sign) || *sign < sign_negative || *sign > sign_positive || (*integral = mdi->monitor_value.integral) < 0 || (*fraction = mdi->monitor_value.fraction) < 0) return 3;
	datetime->year = mdi->datetime.year;
	datetime->month = mdi->datetime.month;
	datetime->day = mdi->datetime.day;
	datetime->hour = mdi->datetime.hour;
	datetime->minute = mdi->datetime.minute;
	datetime->second = mdi->datetime.second;
	return 0;
}

int enqueue_job (char *data, long count, int site)
{
	pthread_mutex_lock (&job_mutex);
	struct job *new;
	if ((new = malloc (sizeof (struct job))) == NULL) return 1;
	new->data = data;
	new->count = count;
	new->next = job_queue;
	job_queue = new->next;
	sem_post (&job_sem);
	pthread_mutex_unlock (&job_mutex);
	return 0;
}

int retrieve_job (struct job **j)
{
	pthread_mutex_lock (&job_mutex);
	*j = job_queue;
	job_queue = job_queue->next;
	pthread_mutex_unlock (&job_mutex);
}

int reuse_sockfd (PER_IO_CONTEXT *pIO)
{
	DWORD err;
	PER_SOCKET_CONTEXT *ck;
	ck = pIO->sock_context;
	memset (pIO->overlapped, 0, sizeof (OVERLAPPED));
	pIO->sock_context = ck;
	pIO->optype = op_reuse;
	if (lp_disconnectex (pIO->sock, &pIO->overlapped, TF_REUSE_SOCKET, 0) == FALSE)
	{
		if ((err = GetLastError ()) == ERROR_IO_PENDING) return 0;
		else
		{
			fprintf (stderr, "DisconnectEx to reuse socket failed. err:%lu, line:%ld\n", err, __LINE__);
			return 1;
		}
	}
	else return 0;
}

int decode_datapdu (char *encode, DWORD bytes, Pdu_t **data_pdu, asn_TYPE_descriptor_t *desc)
{
	//not check arg
	if (decode_incoming (encode, bytes, data_pdu, &asn_DEF_Pdu)) return 1;
	//check
	if (asn_INTEGER2long (&(*data_pdu)->pdu_type, &pdu_type) || pdu_type != datapdu || asn_INTEGER2long (&(*data_pdu)->pdu_packet_number, &pdu_no) || pdu < 0 || asn_INTEGER2long (&(*data_pdu)->site, &site) || (site < site_tyg && site > site_tsg) || asn_INTEGER2long (&(*data_pdu)->pdu_packet.choice.monitor_data_packet.monitor_data_unit_count, item_count) || item_count <= 0 || (new_data = calloc (1, item_count * BAK_FILE_ITEM_LEN)) == NULL)
	{
		free ((*data_pdu));
		return 2;
	}
}

struct job* retrieve_job (pthread_mutex_t *mutex)
{
	struct job *job;
	pthread_mutex_lock (&mutex);
	job = job_queue;
	job_queue = job_queue->next;
	pthread_mutex_unlock (&mutex);
	return job;
}


