#include "server.h"

int check_ip_addr (char *addr)
{
	int dot_count, colon, ipv6;
	char *ch;
	//if (addr == NULL) return -1;
	if (addr[0] == '.' || addr[0] == ':') return 1;
	for (ch = addr, dot_count = 0, colon = 0, ipv6 = 0; *ch != '\0'; ch++)
	{
		if (*ch == '.')
		{
			if (dot_count == 4) return 2;
			else if (ipv6 == 1) return 3;
			else if (*(ch - 1) == '.') return 9;
			dot_count++;
			continue;
		}
		else if (*ch == ':')
		{
			if (!ipv6) ipv6 = 1;
			if (colon == 4) return 4;
			if (dot_count != 0) return 5;
			colon++;
			continue;
		}
		else if (!isalpha (*ch) && !isdigit (*ch))
		{
			return 6;
		}
	}
	if (*(ch - 1) == '.' || *(ch - 1) == ':') return 7;
	if (ch - addr > IP_ADDR_MAX) return 8;
	if (addr == NULL) return -1;
	return 0;
}

int is_valid_path_char (int ch)
{
	switch (ch)
	{
		case '\\':
		case '/':
		case ':':
		case '*':
		case '?':
		case '<':
		case '>':
		case '|':
		case '.':
		return 0;
		default:
		return 1;
	}
}

int check_port (char *port)
{
	char *ch;
	long p;
	//if (port == NULL) return -1;
	for (ch = port, p = 0; *ch != '\0'; ch++)
	{
		if (!isdigit (*ch)) return 1;
		p = p * 10 + *ch - '0';
	}
	if (p > 65535) return 2;
	return 0;
}

int check_path_part (char *part, int bytes, int *part_len)
{
	char *c;
	int has_dot, fn_len, exten_len;
	//if (part == NULL) return -1;
	for (c = part, has_dot = 0, fn_len = 0, exten_len = 0; *c != '\0' && c - part < bytes; c++)
	{
		if (*c == '.')
		{
			if (c == part || has_dot == 1 || c[1] == '\0') return 2;
			else
			{
				has_dot = 1;
				continue;
			}
		}
		else if (is_valid_path_char (*c))
		{
			if (has_dot == 0)
			{
				if (++fn_len > FILE_NAME_MAX) return 3;
			}
			else
			{
				if (++exten_len > FILE_EXTENSION_NAME_MAX) return 4;
			}
		}
		else return 5;
	}
	*part_len = fn_len + exten_len + (has_dot == 1 ? 1 : 0);
	return 0;
}

int check_path (char *pathname)
{
	int has_dot, part_len;
	int path_len, fn_len, exten_len; //path len, filename len, file extension name len
	char *back_slash;
	char *ch, *c;
	//if (pathname == NULL || pathname[0] == '\0') return -1;
	path_len = 0;
	ch = pathname;
	if (ch[1] == ':')
	{
		if (isalpha (ch[0]) && ch[2] == '\\')
		{
			ch += 3;
			path_len += 3;
		}
		else return 1;
	}
	//loop to get '\' character and check part excluding '\'
	for (; ; ch = back_slash + 1)
	{
		if ((back_slash = strchr (ch, '\\')) == NULL)
		{
			if (check_path_part (ch, strlen (ch), &part_len)) return 2;
			else
			{
				if (path_len + part_len > PATH_MAX) return 3;
				else break;
			}
		}
		if (back_slash == ch || back_slash[1] == '\0') return 2;
		if (check_path_part (ch, back_slash - ch, &part_len)) return 2;
		if ((path_len = back_slash - ch + 1) > PATH_MAX) return 3; //add '\' character count
		
	}
	return 0;
}

int init_winsock_library(void)
{
	//init socket library
	WSADATA wsaData;
	if (WSAStartup (MAKEWORD (1,1), &wsaData) != 0) return 1;
	return 0;
}


void *get_GetAcceptExSockAddrs_func_pointer (int sockfd)
{
	DWORD dwBytes;
	LPFN_GETACCEPTEXSOCKADDRS lpfnGetAcceptExSockaddrs;
	GUID guidGetAcceptExSockaddrs = WSAID_GETACCEPTEXSOCKADDRS;  
	if (WSAIoctl (sockfd, SIO_GET_EXTENSION_FUNCTION_POINTER,   
            &guidGetAcceptExSockaddrs, sizeof(guidGetAcceptExSockaddrs),   
            &lpfnGetAcceptExSockaddrs, sizeof (lpfnGetAcceptExSockaddrs),   
            &dwBytes, NULL, NULL)) return NULL;
	return lpfnGetAcceptExSockaddrs;
}

void *get_GetDisconnectEx_func_pointer (int sockfd)
{
	DWORD dwBytes;
	LPFN_DISCONNECTEX lpfnGetDisconnectEx;
	GUID guidDisconnectEx = WSAID_DISCONNECTEX;  
	if (WSAIoctl (sockfd, SIO_GET_EXTENSION_FUNCTION_POINTER,   
            &guidDisconnectEx, sizeof(guidDisconnectEx),   
            &lpfnGetDisconnectEx, sizeof (lpfnGetDisconnectEx),   
            &dwBytes, NULL, NULL)) return NULL;
	return lpfnGetAcceptExSockaddrs;
}

void *get_AcceptEx_func_pointer (int sockfd)
{
	LPFN_ACCEPTEX m_lpfnAcceptEx;
	GUID guidAcceptEx = WSAID_ACCEPTEX;
	DWORD dwBytes = 0;
	if (WSAIoctl (sockfd, SIO_GET_EXTENSION_FUNCTION_POINTER, &guidAcceptEx, sizeof(guidAcceptEx), &m_lpfnAcceptEx, sizeof(m_lpfnAcceptEx), &dwBytes, NULL, NULL)) return NULL;
	return lpfnAcceptEx;
}

int init_server_socket_stream (char *serv_addr, char *serv_type, SOCKET *socket)
{
	struct sockaddr_storage remoteaddr;
	socklen_t addrlen;
	struct addrinfo hints, *ai, *p;
	addrlen = sizeof (remoteaddr);
	int yes = TRUE;
	SOCKET sock;

	//if (serv_type == NULL) return -1;
	memset (&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	//hints.ai_protocol = IPPROTO_TCP;
	if (getaddrinfo (serv_addr, serv_type, &hints, &ai) != 0) return 1;

	for(p = ai; p != NULL; p = p->ai_next)
	{
		//sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		sock = WSASocket(p->ai_family, p->ai_socktype, p->ai_protocol, NULL, 0, WSA_FLAG_OVERPAPPED);
		if (sock == INVALID_SOCKET) continue;

		// lose the pesky "address already in use" error message
		if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)&yes, sizeof(int)) == SOCKET_ERROR)
		{
			closesocket (sock);
			continue;
		}
		if (bind(sock, p->ai_addr, p->ai_addrlen) == SOCKET_ERROR)
		{
			closesocket(sock);
			continue;
		}
		break;
	}
	freeaddrinfo (ai); // all done with this
	if (p == NULL) return 2;
	if (listen(sock, SOMAXCONN) == SOCKET_ERROR) //SOMAXCONN defined by WinSock2.h, Maximum queue length specifiable by listen
	{
		closesocket (sock);
		return 3;
	}
	*socket = sock;
	return 0;
}

int init_server_overlapped_socket_stream (char *serv_addr, char *serv_type, SOCKET *socket, int use_overlapped)
{
	struct sockaddr_storage remoteaddr;
	socklen_t addrlen;
	struct addrinfo hints, *ai, *p;
	addrlen = sizeof (remoteaddr);
	int yes = TRUE;
	SOCKET sock;
	int rv;

	//if (serv_type == NULL) return -1;
	memset (&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	//hints.ai_protocol = IPPROTO_TCP;
	if ((rv = getaddrinfo (serv_addr, serv_type, &hints, &ai)) != 0) return 1;

	for(p = ai; p != NULL; p = p->ai_next)
	{
		//sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (use_overlapped) sock = WSASocket(p->ai_family, p->ai_socktype, p->ai_protocol, NULL, 0, WSA_FLAG_OVERLAPPED); //win api, last arg to use overlapped socket
		else sock = socket (p->ai_family, p->ai_socktype, p->ai_protocol);
		if (sock == INVALID_SOCKET) continue;

		// lose the pesky "address already in use" error message
		if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)&yes, sizeof(int)) == SOCKET_ERROR)
		{
			closesocket (sock);
			continue;
		}
		if (bind(sock, p->ai_addr, p->ai_addrlen) == SOCKET_ERROR)
		{
			closesocket(sock);
			continue;
		}
		break;
	}
	freeaddrinfo (ai); // all done with this
	if (p == NULL) return 2;
	if (listen(sock, SOMAXCONN) == SOCKET_ERROR) //SOMAXCONN defined by WinSock2.h, Maximum queue length specifiable by listen
	{
		closesocket (sock);
		return 3;
	}
	*socket = sock;
	return 0;
}

void get_processor_num (DWORD *num) //according to num to processor, create 2 * num or 2 * num + 2 of thread
{
	
	SYSTEM_INFO si;  
	GetSystemInfo(&si);  

	*num = si.dwNumberOfProcessors;  
}

int create_socket_context_bind_completion_port (HANDLE *completion_port, SOCKET sock, PER_SOCKET_CONTEXT *listen_sock_context, PER_SOCKET_CONTEXT **sock_context)
{
	
	PER_SOCKET_CONTEXT *context;
	if (completion_port == NULL || sock_context == NULL) return -1;
	if ((context = calloc (1, sizeof (PER_SOCKET_CONTEXT))) == NULL) return 1;
	context->sock = sock;
	//context->client_addr = listen_sock.client_addr;
	if (CreateIoCompletionPort ((HANDLE)sock, completion_port, (ULONG_PTR) context, 0) == NULL)
	{
		free (context);
		return 2;
	}
	*sock_context = context;
	return 0;
}

int init_socket_lib ()
{
	WSADATA wsaData;
	if (WSAStartup (MAKEWORD (1,1), &wsaData) != 0) return 1;
	return 0;
}
