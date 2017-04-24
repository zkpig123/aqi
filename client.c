#include "client.h"

/*
 * give file name and open it, return HANDLE or INVALID_HANDLE_VALUE,
 * mode is  negative, zero, positive correspondingly meaning other
 * process could read, could not read or write, could write
 * to the opened file,
 */
int open_file_handle (char *fn, DWORD access_mode, DWORD share_mode, HANDLE *hand)
{
	HANDLE fh;
	DWORD ret;
	//if (fn == NULL) return -1;
	switch (share_mode)
	{
		case FILE_SHARE_READ: case FILE_SHARE_WRITE: case FILE_SHARE_READ | FILE_SHARE_WRITE: case 0:
		break;
		default:
		return -1;
	}
	switch (access_mode)
	{
		case GENERIC_EXECUTE: case GENERIC_READ: case GENERIC_WRITE: case GENERIC_READ | GENERIC_WRITE:
		break;
		default:
		return -1;
	}
	fh = CreateFile (fn, access_mode, share_mode, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL); //try open it
	if (fh != INVALID_HANDLE_VALUE)
	{
		*hand = fh;
		return 0;
	}
	ret = GetLastError ();
	if (ret == ERROR_FILE_NOT_FOUND) //file not exist
	{
		//create it
		if ((fh = CreateFile (fn, access_mode, share_mode, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE)
		{
			return 1; //create file failed
		}
		else
		{
			*hand = fh;
			return 0;
		}
	}
	else if (ret == ERROR_ACCESS_DENIED) //file attribute rdonly or not enough privilege to open it
	{
		DWORD dwAttr;
		dwAttr = FILE_ATTRIBUTE_NORMAL;
		if (!SetFileAttributes (fn, dwAttr)) return 2; //set file attributes failed
		if ((fh = CreateFile (fn, access_mode, share_mode, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE) return 3; //reopen failed
		else
		{
			*hand = fh;
			return 0;
		}
	}
	else return 4;
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

/*
 * check whether given path is a valid path which should be
 * formatted as filename(xxx.xx or xxx, x should be digit,
 * alpha, or underscore) or full path(c:\a.txt)
 */
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

int lock_file_region (HANDLE fh, DWORD pos_low, DWORD pos_high, DWORD lock_low, DWORD lock_high)
{
	//if (fh == INVALID_HANDLE_VALUE || (lock_low == 0 && lock_high == 0)) return -1;
	if (!LockFile (fh, pos_low, pos_high, lock_low, lock_high))
	{
		fprintf (stderr, "LockFile file failed. exit. line:%ld\n", __LINE__);
		exit (3);
	}
	return 0;
}

int unlock_file_region (HANDLE fh, DWORD pos_low, DWORD pos_high, DWORD lock_low, DWORD lock_high)
{
	//if (fh == INVALID_HANDLE_VALUE || (lock_low == 0 && lock_high == 0)) return -1;
	if (!UnlockFile (fh, pos_low, pos_high, lock_low, lock_high))
	{
		fprintf (stderr, "UnlockFile file failed. exit. err:%lu, line:%ld\n", GetLastError (), __LINE__);
		exit (3);
	}
	return 0;
}
