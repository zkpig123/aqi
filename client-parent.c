#include "client.h"

#define SHARE_FILE_NAME "client_share_file"
#define PROCESS_RESULT_FILE_INTERVAL 10
#define MONITOR_VALID_TIME_RANGE 3600 * 24 //monitor time should range between given range from current time, in seconds, 24 h
#define RESULT_LINE_LEN_MAX 128
#define RESULT_FILE_BUF_LEN 1920
#define OPEN_RESULT_FILE_FAIL_INTERVAL 2
#define FILE_NAME_MAX 64
#define FILE_EXTENSION_NAME_MAX 12
#define PROCESS_ID_LEN_MAX 16 //HEX
#define ARG_BUF_LEN sizeof ("progname") - 1 + 1 + SITENAMEMAX + PATH_MAX + 1 + PROCESS_ID_LEN_MAX + 1 + IP_ADDR_MAX + 1 + 5 + 1 + sizeof (MAP_NAME) + 1 + 10 //5 is port len, 1 for siteid, 10 for protect

static int get_file_pos (HANDLE fh, LONG *pos_low, PLONG pos_high);
static int share_memory (char *fn, HANDLE *fh, HANDLE *map_obj, char * map_name, uint64_t map_file_size, DWORD map_obj_mode, LPVOID *map_addr, DWORD map_mode, uint64_t file_offset, SIZE_T mapping_size);
static int create_process (char *arg, PROCESS_INFORMATION *pi, STARTUPINFO *si, HANDLE *write_pipe, HANDLE *read_pipe);
static void open_bak_file (char *fn1, char *fn2);
int process_result_file (HANDLE fh_result, HANDLE fh_bak, HANDLE fh_bak_bin, char *buf, size_t buf_len);
int check_result_line_write_bak_file (char *line, size_t len, HANDLE bak_file, HANDLE bak_bin_file);
static void check_arg (int argc, char *argv[]);
static int create_pipe (HANDLE *read_pipe, HANDLE *write_pipe);

static int site;
static HANDLE fh_result, fh_bak, fh_bak_bin, fh_share;
static char last_result_md5[MD5LEN], cur_result_md5[MD5LEN]; //last md5 of result ini file
static unsigned long last_monitor_time[type_doas_mercury_lamp_pos - type_so2];
static unsigned long last_time_so2, last_time_no2, last_time_co, last_time_pm10, last_time_pm25, last_time_o3, last_time_wind_velocity, last_time_ambient_temperature, last_time_humidity, last_time_atom_pressure, last_time_wind_direction, last_time_room_temperature, last_time_doas_light_insensity, last_time_doas_monitor_status, last_time_doas_integral_time, last_time_doas_average_count, last_time_doas_optical_path, last_time_doas_so2_sig_noise_ratio, last_time_doas_no2_sig_noise_ratio, last_time_doas_o3_sig_noise_ratio, last_time_doas_sig_noise_ratio, last_time_doas_pda_status, last_time_doas_elec_machine_status, last_time_doas_adjust_poll_pos; //last monitor time of type

/*
 * argc1 is sitename, argc2 is file path of result ini file, argc3 is result backup file path, argc4 is result backup bin file path
 */
int main (int argc, char *argv[])
{
	int ret;
	LONG pos_low;
	LONG pos_high;
	HANDLE write_pipe[2], read_pipe[2]; //two pipe to comminucate between two process
	HANDLE child;
	PROCESS_INFORMATION pi;
	DWORD cur_process_id;
	DWORD bytes;
	STARTUPINFO si;
	char arg[ARG_BUF_LEN];
	char result_buf[RESULT_FILE_BUF_LEN];

	if (setvbuf (stdout, NULL, _IONBF, 0))
	{
		fprintf (stderr, "setvbuf for stdout failed. exit. err:%lu, line:%ld\n", GetLastError (), __LINE__);
		exit (1);
	}
	check_arg (argc, argv);
	open_bak_file (argv[3], argv[4]); //open log file and set file pointer to eof
	get_file_pos (fh_bak_bin, &pos_low, &pos_high); //get file pos to lock result bak bin file which is shared between process
	lock_file_region (fh_bak_bin, (DWORD)pos_low, (DWORD)pos_high, RESULT_BAK_ITEM_BYTES % UINT32_MAX, RESULT_BAK_ITEM_BYTES / UINT32_MAX); //lock bytes of item length from end of result backup file path
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

	//pass arg : progname, siteid, result_bak_bin_file_path, processid_of_parent, server_ip_addr, server_port, map_name
	if ((ret = snprintf (arg, ARG_BUF_LEN, "%s %s %s %lu %s %s %s", "progname", argv[1], argv[4], cur_process_id, argv[5], argv[6], MAP_NAME)) < 0 || ret + 1 > ARG_BUF_LEN)
	{
		fprintf (stderr, "buffer for child arg is not enough. exit. err:%lu, line:%ld\n", GetLastError (), __LINE__);
		exit (1);
	}
	//create process
	create_process (arg, &pi, &si, write_pipe, read_pipe);
	while (1)
	{
		//check and create child process
		if ((child = OpenProcess (PROCESS_QUERY_INFORMATION, FALSE, pi.dwProcessId)) == NULL) create_process (arg, &pi, &si, write_pipe, read_pipe);
		else CloseHandle (child);

		//open result file
		while (open_file_handle (argv[2], GENERIC_READ, FILE_SHARE_READ, &fh_result)) sleep (OPEN_RESULT_FILE_FAIL_INTERVAL);
		process_result_file (fh_result, fh_bak, fh_bak_bin, result_buf, RESULT_FILE_BUF_LEN); //close file during this func
		sleep (PROCESS_RESULT_FILE_INTERVAL);
	}
}

int process_result_file (HANDLE fh_result, HANDLE fh_bak, HANDLE fh_bak_bin, char *buf, size_t buf_len)
{
	DWORD bytes;
	size_t line_len;
	char *cur, *carriage;
	char line[RESULT_LINE_LEN_MAX];
	if (buf == NULL || buf_len <= 0) return -1;
	//read result content
	if (!ReadFile (fh_result, buf, buf_len, &bytes, NULL))
	{
		fprintf (stderr, "ReadFile to read result file err, exit, err:%lu, line:%ld\n", GetLastError (), __LINE__);
		exit (3);
	}
	if (!CloseHandle (fh_result))
	{
		fprintf (stderr, "close result file err, exit, err:%lu, line:%ld\n", GetLastError (), __LINE__);
		exit (3);
	}
	//calc md5 of result file
	if (bytes == 0) return 0; //no content
	if (bytes == buf_len)
	{
		fprintf (stderr, "buf for result file content is not enough, exit, err:%lu, line:%ld\n", GetLastError (), __LINE__);
		exit (3);
	}
	else buf[bytes] = '\0';
	calc_file_md5 (buf, bytes, cur_result_md5);
	//result file content not change from last time
	if (!comp_file_md5 (cur_result_md5, last_result_md5)) return 0;
	cur = buf;
	do
	{
		if ((carriage = strchr (cur, '\r')) == NULL)
		{
			line_len = buf + bytes - cur;
			strncpy (line, cur, line_len);
			line[line_len] = '\0';
			cur = buf + bytes;
		}
		else
		{
			line_len = carriage - cur;
			strncpy (line, cur, line_len);
			line[line_len] = '\0';
			cur = carriage + 2;
		}
		if (line_len < 18) continue;
		if (check_result_line_write_bak_file (line, line_len, fh_bak, fh_bak_bin))
		{
			fprintf (stderr, "line from result invalid. line:%s. line:%ld\n", line, __LINE__);
		}
	} while (cur < buf + bytes);
	strncpy (last_result_md5, cur_result_md5, MD5LEN);
}

int check_result_line_write_bak_file (char *line, size_t len, HANDLE bak_file, HANDLE bak_bin_file)
{
	struct tm result_tm;
	time_t cur_time, result_time;
	DWORD bytes;
	uint64_t time_num;
	uint32_t integral_num, fraction_num;
	char *equal, *hyphen1, *hyphen2, *colon1, *colon2, *colon3, *blank1, *blank2, *dot;
	long type, year, month, day, hour, minute, second, integral, fraction;
	e_sign sign;
	char *ch;
	char item[RESULT_BAK_ITEM_BYTES];
	//here not check arg

	if (line[0] == '[') return 0; //not data line
	if ((equal = strchr (line, '=')) == NULL || equal - line == len - 1) return 1;
	if ((hyphen1 = strchr (equal + 1, '-')) == NULL || hyphen1 - line == len - 1 || (hyphen2 = strchr (hyphen1 + 2, '-')) == NULL || hyphen2 - line == len - 1 || (blank1 = strchr (hyphen2 + 2, ' ')) == NULL || blank1 - line == len - 1 || (colon1 = strchr (blank1 + 2, ':')) == NULL || colon1 - line == len - 1 || (colon2 = strchr (colon1 + 2, ':')) == NULL || colon2 - line == len - 1 || (blank2 = strchr (colon2 + 2, ' ')) == NULL || blank2 - line == len - 1) return 1;
	if ((dot = strchr (blank2 + 1, '.')) == NULL || dot - line == len - 1) fraction = 0;
	else
	{
		//check fraction
		for (ch = dot + 1, fraction = 0; *ch != '\0'; ch++)
		{
			if (!isdigit (*ch)) return 2;
			else fraction = fraction * 10 + *ch - '0';
		}
	}
	*equal = '\0';
	type = get_asn_type_from_name (line);
	if (type == type_invalid_type) return 3;
	for (year = 0, ch = equal + 1; ch < hyphen1; ch++)
	{
		if (!isdigit (*ch)) return 4;
		else year = year * 10 + *ch - '0';
	}
	for (month = 0, ch = hyphen1 + 1; ch < hyphen2; ch++)
	{
		if (!isdigit (*ch)) return 4;
		else month = month * 10 + *ch - '0';
	}
	for (day = 0, ch = hyphen2 + 1; ch < blank1; ch++)
	{
		if (!isdigit (*ch)) return 4;
		else day = day * 10 + *ch - '0';
	}
	for (hour = 0, ch = blank1 + 1; ch < colon1; ch++)
	{
		if (!isdigit (*ch)) return 4;
		else hour = hour * 10 + *ch - '0';
	}
	for (minute = 0, ch = colon1 + 1; ch < colon2; ch++)
	{
		if (!isdigit (*ch)) return 4;
		else minute = minute * 10 + *ch - '0';
	}
	for (second = 0, ch = colon2 + 1; ch < blank2; ch++)
	{
		if (!isdigit (*ch)) return 4;
		else second = second * 10 + *ch - '0';
	}
	if (year < 0 || month < 0 || day < 0 || check_year_month_day (year, month, day) || hour < 0 || hour > 23 || minute < 0 || minute > 59 || second < 0 || second > 61) return 4;
	time_num = year * 10000000000 + month * 100000000 + day * 1000000 + hour * 10000 + minute * 100 + second;
	if (type != type_doas_so2_sig_noise_ratio && type != type_doas_no2_sig_noise_ratio)
	{
		if (last_monitor_time[type] == time_num) return 0;
		else last_monitor_time[type] = time_num;
	}
	else last_monitor_time[type] = time_num;
	if ((cur_time = time (&cur_time)) == (time_t) -1)
	{
		fprintf (stderr, "get cur time err. exit. line:%ld\n", __LINE__);
		exit (2);
	}
	result_tm.tm_year = year - 1900;
	result_tm.tm_mon = month - 1;
	result_tm.tm_mday = day;
	result_tm.tm_hour = hour;
	result_tm.tm_min = minute;
	result_tm.tm_sec = second;
	result_tm.tm_isdst = 0;
	if ((result_time = mktime (&result_tm)) == (time_t) -1)
	{
		fprintf (stderr, "mktime func failed. exit. line:%ld\n", __LINE__);
		exit (2);
	}
	time_t t;
	if ((t = result_time - cur_time) < 0) t = -t;
	if (t > MONITOR_VALID_TIME_RANGE) return 0;
	
	if (blank2[1] == '-')
	{
		sign = sign_negative;
		ch = blank2 + 2;
	}
	else if (blank2[1] == '+')
	{
		sign = sign_positive;
		ch = blank2 + 2;
	}
	else
	{
		sign = sign_positive;
		ch = blank2 + 1;
	}
	int haszero = 0;
	size_t dif;
	for (integral = 0; (dif = ch - line) < len && *ch != '.'; ch++)
	{
		if (!isdigit (*ch)) return 4;
		else integral = integral * 10 + *ch - '0';
	}
	if (dif < len)
	{
		ch++;
		for (fraction = 0; (dif = ch - line) < len; ch++)
		{
			if (!isdigit (*ch)) return 4;
			else fraction = fraction * 10 + *ch - '0';
		}
	}
	else fraction = 0;
	//write to bak file
	if (!WriteFile (fh_bak, line, len, &bytes, NULL) || !WriteFile (fh_bak, "\r\n", 2, &bytes, NULL))
	{
		fprintf (stderr, "WriteFile to write bak file failed. exit. err:%lu, line:%ld\n", GetLastError (), __LINE__);
		exit (2);
	}

	//write to bak bin file
	integral_num = integral;
	fraction_num = fraction;
	item[0] = type;
	item[1] = time_num >> 56;
	item[2] = (time_num << 8) >> 56;
	item[3] = (time_num << 16) >> 56;
	item[4] = (time_num << 24) >> 56;
	item[5] = (time_num << 32) >> 56;
	item[6] = (time_num << 40) >> 56;
	item[7] = (time_num << 48) >> 56;
	item[8] = (time_num << 56) >> 56;
	item[9] = sign;
	item[10] = integral_num >> 24;
	item[11] = (integral_num << 8) >> 24;
	item[12] = (integral_num << 16) >> 24;
	item[13] = (integral_num << 24) >> 24;
	item[14] = fraction_num >> 24;
	item[15] = (fraction_num << 8) >> 24;
	item[16] = (fraction_num << 16) >> 24;
	item[17] = (fraction_num << 24) >> 24;
	if (!WriteFile (fh_bak_bin, item, RESULT_BAK_ITEM_BYTES, &bytes, NULL))
	{
		fprintf (stderr, "WriteFile to write bak bin file failed. exit. err:%lu, line:%ld\n", GetLastError (), __LINE__);
		exit (2);
	}
	//lock next part of bak bin file
	LONG pos_low, pos_high;
	DWORD lock_low, lock_high;
	get_file_pos (fh_bak_bin, &pos_low, &pos_high);
	lock_file_region (fh_bak_bin, (DWORD)pos_low, (DWORD)pos_high, RESULT_BAK_ITEM_BYTES % UINT32_MAX, RESULT_BAK_ITEM_BYTES / UINT32_MAX);
	if (pos_low >= RESULT_BAK_ITEM_BYTES) pos_low -= RESULT_BAK_ITEM_BYTES;
	else
	{
		pos_low = 0;
		pos_high -= (RESULT_BAK_ITEM_BYTES - pos_low);
	}
	unlock_file_region (fh_bak_bin, (DWORD)pos_low, (DWORD)pos_high, RESULT_BAK_ITEM_BYTES, RESULT_BAK_ITEM_BYTES / UINT32_MAX);
	return 0;
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
	if (!CreateProcess (CHILD_PROCESS_NAME, arg, NULL, NULL, TRUE, 0, NULL, NULL, si, pi))
	{
		fprintf (stderr, "createprocess failed. err:%lu, line:%ld\n", GetLastError (), __LINE__);
		return 1;
	}
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

static void open_bak_file (char *fn, char *fn_bin)
{
	if (open_file_handle (fn, GENERIC_WRITE, FILE_SHARE_READ, &fh_bak))
	{
		fprintf (stderr, "open result bak file failed. line:%ld\n", __LINE__);
		exit (3);
	}
	if (SetFilePointer (fh_bak, 0, NULL, FILE_END) == INVALID_SET_FILE_POINTER)
	{
		fprintf (stderr, "wind to end of result bak file failed. exit. err:%lu. line:%ld\n", GetLastError (), __LINE__);
		exit (3);
	}
	if (open_file_handle (fn_bin, GENERIC_WRITE, FILE_SHARE_READ, &fh_bak_bin))
	{
		fprintf (stderr, "open result bak bin file failed. line:%ld\n", __LINE__);
		exit (3);
	}
	if (SetFilePointer (fh_bak_bin, 0, NULL, FILE_END) == INVALID_SET_FILE_POINTER)
	{
		fprintf (stderr, "wind to end of result bak bin file failed. exit. err:%lu. line:%ld\n", GetLastError (), __LINE__);
		exit (3);
	}
	
}

static void check_arg (int argc, char *argv[])
{
	char *ch;
	long port;
	if (argc != 7)
	{
		fprintf (stderr, "usage: progname sitename result_ini_file_path, result_file_bak_file_path, result_file_bak_bin_file_path, server_addr, server_port. given argc is %d. exit. line:%ld\n", argc, __LINE__);
		exit (1);
	}
	if ((site = get_asn_site_type_from_name (argv[1])) == site_invalid_site)
	{
		fprintf (stderr, "invalid site name. exit. line:%ld\n", __LINE__);
		exit (1);
	}
	if (check_path (argv[2]))
	{
		fprintf (stderr, "invalid result path. exit. line:%ld\n", __LINE__);
		exit (1);
	}
	if (check_path (argv[3]))
	{
		fprintf (stderr, "invalid log path. exit. line:%ld\n", __LINE__);
		exit (1);
	}
	if (check_path (argv[4]))
	{
		fprintf (stderr, "invalid log bin path. exit. line:%ld\n", __LINE__);
		exit (1);
	}
	if (check_ip_addr (argv[5]))
	{
		fprintf (stderr, "invalid ip addr. exit. line:%ld\n", __LINE__);
		exit (1);
	}
	if (check_port (argv[6]))
	{
		fprintf (stderr, "invalid port. exit. line:%ld\n", __LINE__);
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
