#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include "aqi.h"
#include <sys/time.h>
#include "asn.h"
#include "md5.h"
#include <unistd.h>

#define RESULT_BAK_ITEM_BYTES 18
#define FILE_NAME_MAX 64
#define FILE_EXTENSION_NAME_MAX 12
#define PROCESS_RESULT_FILE_INTERVAL 10
#define MONITOR_VALID_TIME_RANGE 3600 * 24 //monitor time should range between given range from current time, in seconds, 24 h
#define RESULT_LINE_LEN_MAX 128
#define RESULT_FILE_BUF_LEN 1920
#define OPEN_RESULT_FILE_FAIL_INTERVAL 2
#define CHILD_PROCESS_NAME "child.exe"
#define PROCESS_ID_LEN_MAX 16 //HEX
#define RESULT_BAK_ITEM_BYTES 18
#define MAP_SIZE 1024 //bytes
#define MAP_MODE FILE_MAP_COPY
#define MAP_OBJECT_MODE PAGE_READWRITE
#define MAP_NAME "map_between_client_parent_child"

int lock_file_region (HANDLE fh, DWORD pos_low, DWORD pos_high, DWORD lock_low, DWORD lock_high);
int unlock_file_region (HANDLE fh, DWORD pos_low, DWORD pos_high, DWORD lock_low, DWORD lock_high);
int check_port (char *port);
int check_ip_addr (char *addr);
int open_file_handle (char *fn, DWORD access_mode, DWORD share_mode, HANDLE *hand);
int check_path (char *pathname);
int check_path_part (char *part, int bytes, int *part_len);
int is_valid_path_char (int ch);

