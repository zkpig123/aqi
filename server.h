#include "Pdu.h"
#include <mysql.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <mswsock.h>
#include <windows.h>
#include "aqi.h"
#include <sys/time.h>
#include "asn.h"
#include "md5.h"
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>

enum OPERATION_TYPE {op_accept, op_recv, op_send, op_reuse};

typedef struct _PER_SOCKET_CONTEXT{
	SOCKET sock;
	//sockaddr client_addr;
	//PER_IO_CONTEXT io_context[]; //array of io context
} PER_SOCKET_CONTEXT;

typedef struct _PER_IO_CONTEXT{
	OVERLAPPED overlapped;
	SOCKET sock;
	WSABUF wsabuf;
	char *buf; //a pointer to a buf whose length should be at least MAX_DATA_PDU_ENCODE_LEN
	//char buf[MAX_DATA_PDU_ENCEDE_LEN];
	enum OPERATION_TYPE optype; //recv or accept or send
	PER_SOCKET_CONTEXT *sock_context;
} PER_IO_CONTEXT;

int check_ip_addr (char *addr);
int check_port (char *port);
int is_valid_path_char (int ch);
int check_path_part (char *part, int bytes, int *part_len);
int init_winsock_library(void);
void *get_GetAcceptExSockAddrs_func_pointer (int sockfd);
int check_path (char *pathname);
void *get_GetDisconnectEx_func_pointer (int sockfd);
void *get_AcceptEx_func_pointer (int sockfd);
int init_server_socket_stream (char *serv_addr, char *serv_type, SOCKET *socket);
int init_server_overlapped_socket_stream (char *serv_addr, char *serv_type, SOCKET *socket, int use_overlapped);
void get_processor_num (DWORD *num); //according to num to processor, create 2 * num or 2 * num + 2 of thread
int create_socket_context_bind_completion_port (HANDLE *completion_port, SOCKET sock, PER_SOCKET_CONTEXT *listen_sock_context, PER_SOCKET_CONTEXT **sock_context);
int init_socket_lib ();
