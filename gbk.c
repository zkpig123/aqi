#include "aqi.h"
#include <stdio.h>
#include <sys/types.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>

//character count of gbk string not include terminating null, unprintable character is considered invalid, bytes not including the terminating null,return -1 if arg invalid, return -2 if character invalid or unprintable character
ssize_t char_count_gbk (char *str, size_t bytes)
{
	ssize_t count;
	unsigned char *start, *end;
	if (NULL == str || bytes <= 0) return -1;
	size_t len;
	len = strlen (str);
	if (len < bytes) bytes = len;
	start = (unsigned char*)str;
	end = start + bytes - 1;
	count = 0;
	for (unsigned char *ch = start; ch <= end ; )
	{
		//single byte character
		if (ch[0] <= 0x7F)
		{
			if (ch[0] < 0x20 && ch[0] != '\t' && ch[0] != '\n' && ch[0] != '\r' && ch[0] != '\v' && ch[0] != '\f') return -2;
			count++;
			ch++;
			continue;
		}

		//two-byte character
		if (end == ch) return -2;
		if (ch[0] < 0x81 || ch[0] > 0xFE || ch[1] < 0x40 || ch[1] > 0xFE) return -2;
		count++;
		ch+=2;
	}

	return count;
}

//character count of gbk string not including terminating null, unprintable character is valid, bytes not including the terminating null,return -1 if arg invalid, return -2 if character invalid
ssize_t char_count_gbk_bin (char *str, size_t bytes)
{
	ssize_t count;
	if (NULL == str || bytes <= 0) return -1;
	size_t len;
	len = strlen (str);
	if (len < bytes) bytes = len;
	unsigned char *start = (unsigned char*)str;
	unsigned char *end = start + bytes - 1;
	count = 0;
	for (unsigned char *ch = start; ch <= end; )
	{
		//single byte character
		if (ch[0] <= 0x7F)
		{
			count++;
			ch++;
			continue;
		}

		//two-byte character
		if (end == ch) return -2;
		if (ch[0] < 0x81 || ch[0] > 0xFE || ch[1] < 0x40 || ch[1] > 0xFE) return -2;
		count++;
		ch+=2;
	}

	return count;
}

//byte count of gbk string non including terminating nul,bytes not including the terminating null return -1 if arg invalid, return -2 if character invalid or unprintable
ssize_t byte_count_gbk (char *str, size_t bytes)
{
	if (NULL == str || bytes <= 0) return -1;
	unsigned char *start = (unsigned char*)str;
	unsigned char *end = start + bytes - 1;
	unsigned char *ch;
	size_t len;
	len = strlen (str);
	if (len < bytes) bytes = len;

	for (ch = start; ch <= end ; )
	{
		//single byte character
		if (ch[0] <= 0x7F)
		{
			if (ch[0] < 0x20 && ch[0] != '\t' && ch[0] != '\n' && ch[0] != '\r' && ch[0] != '\v' && ch[0] != '\f') return -2;
			ch++;
			continue;
		}

		//two-byte character
		if (end == ch) return -2;
		if (ch[0] < 0x81 || ch[0] > 0xFE || ch[1] < 0x40 || ch[1] > 0xFE) return -2;
		ch+=2;
	}

	return (ch - start);
}

//byte count of gbk string not including terminating null,bytes not including the terminating null, return -1 if arg invalid, return -2 if character invalid
ssize_t byte_count_gbk_bin (char *str, size_t bytes)
{
	if (NULL == str || bytes <= 0) return -1;
	unsigned char *start = (unsigned char*)str;
	unsigned char *end = start + bytes - 1;
	unsigned char *ch;
	size_t len;
	len = strlen (str);
	if (len < bytes) bytes = len;

	for (ch = start; ch <= end; )
	{
		//single byte character
		if (ch[0] <= 0x7F)
		{
			ch++;
			continue;
		}

		//two-byte character
		if (end == ch) return -2;
		if (ch[0] < 0x81 || ch[0] > 0xFE || ch[1] < 0x40 || ch[1] > 0xFE) return -2;
		ch+=2;
	}

	return (ch - start);
}

//check whether encoding of specified gbk string is valid, bytes not including the terminating null,return 0 if valid, return 1 if containing unprintable character,invalid when return other value
int char_check_gbk (char *str, size_t bytes)
{
	if (NULL == str || bytes <= 0) return 2;
	unsigned char *start = (unsigned char*)str;
	unsigned char *end = start + bytes - 1;
	int have_unprintable_char = 0;
	size_t len;
	len = strlen (str);
	if (len < bytes) bytes = len;
	for (unsigned char *ch = start; ch <= end ;)
	{
		if (ch[0] <= 0x7F)
		{
			if (ch[0] < 0x20 && ch[0] != '\t' && ch[0] != '\n' && ch[0] != '\r' && ch[0] != '\v' && ch[0] != '\f') have_unprintable_char = 1;
			ch++;
		}
		else
		{
			if (end == ch) return 2;
			if (ch[0] < 0x81 || ch[0] > 0xFE || ch[1] < 0x40 || ch[1] > 0xFE) return 3;
			ch+=2;
		}
	}

	if (have_unprintable_char == 1) return 1;
	return 0;
}

/*
return first occurency position of target character in str,position is 
count in character but not byte,first position is 1 not 0, and validate 
check is performed for target but not str, return 0 if not exist, return 
-1 if arg invalid, return -2 if target is has invalid character encoding 
bytes, return -3 if not found
*/
ssize_t char_pos_gbk (char *str, size_t bytes1, char *target, size_t bytes2)
{
	size_t len1, len2;
	len1 = strlen (str);
	len2 = strlen (target);
	if (len1 < bytes1) bytes1 = len1;
	if (len2 < bytes2) bytes2 = len2;
	if (NULL == str || bytes1 <= 0 || bytes2 <= 0 || NULL == target || bytes1 < bytes2 ) return -1;
	unsigned char *start = (unsigned char*)str;
	unsigned char *end = start + bytes1 - 1 - bytes2;
	ssize_t pos;
	if (char_check_gbk (target, bytes2)) return -2;
	for (unsigned char *ch = start, pos = 1; ch <= end;)
	{

		if (!strncmp ((char*)ch, target, bytes2)) return pos;
		if (ch[0] <= 0x7F) ch++;
		else ch += 2;
		pos++;
	}
	return -3;
}

/*
 * return first occurency position of target character in str,position is count 
 * in byte,first byte is 0 not 1, and validate check is performed for target but not str 
 * return -1 if arg invalid, return -2 if target has invalid encoding, return -3
 * if not exist
*/
ssize_t byte_pos_gbk (char *str, size_t bytes1, char *target, size_t bytes2)
{
	if (NULL == str || bytes1 <= 0 || bytes2 <= 0 || NULL == target || bytes1 < bytes2 ) return -1;
	unsigned char *start = (unsigned char*)str;
	unsigned char *end = start + bytes1 - 1 - bytes2;
	if (char_check_gbk (target, bytes2)) return -2;
	for (unsigned char *ch = start; ch <= end;)
	{
		if (!strncmp ((char*)ch, target, bytes2)) return (ssize_t) (ch - start);
		if (ch[0] <= 0x7F) ch++;
		else ch += 2;
	}
	return -3;
}

/*
get a printable character from a stream stream,and return value 
is in uint16_t form,return 0x00 03(byte sequence) if stream is null,
return 0x00 01 for eof, return 0x02 02 for unprintable character,return 0x01 01
while eof and fseek back error,return 0x00 02 for invalid character, return 0x01 02 
for invalid character and fseek back error
*/
uint16_t fgetc_gbk (FILE *stream)
{
	int byte1, byte2;
	byte1 = byte2 = 0;
	uint16_t ret;
	if (stream == NULL)
	{
		*(char *)&ret = 0x00;
		*((char *)&ret + 1) = 0x03;
		return ret; //return 0x00 03 if stream is null
	}
	if ((byte1 = fgetc (stream)) == EOF)
	{
		*(char *)&ret = 0;
		*((char *)&ret + 1) = 1;
		return ret; //return 0x00 01 for eof
	}
	if (byte1 <= 0x7F)
	{
		if (byte1 < 0x20 && byte1 != '\t' && byte1 != '\n' && byte1 != '\r' && byte1 != '\v' && byte1 != '\f')
		{
			*(char*)&ret = 2;
			*((char*)&ret + 1) = 2;
			return ret; //return 0x02 02 for unprintable character
		}
		*(char *)&ret = (char)byte1;
		*((char *)&ret + 1) = 0;
		return ret;
	}
	if ((byte2 = fgetc (stream)) == EOF)
	{
		if (fseek (stream, (long) -1, SEEK_CUR))
		{
			return 0x0101; //for eof and fseek back error
		}
		*(char *)&ret = 0;
		*((char *)&ret + 1) = 1;
		return ret;
		//return 0x00 01 for eof
	}
	if ((unsigned)byte1 < 0x81 || (unsigned)byte1 > 0xFE || (unsigned)byte2 < 0x40 || (unsigned)byte2 > 0xFE)
	{
		if (fseek (stream, (long) -2, SEEK_CUR))
		{
			*(char *)&ret = 1;
			*((char *)&ret + 1) = 2;
			return ret; //return 0x01 02 for character invalid and fseek back error
		}
		*(char *)(&ret) = 0;
		*((char *)&ret + 1) = 2;
		return ret; // return 0x00 02 for character invalid
	}
	*(char *)&ret = byte1;
	*((char *)&ret + 1) = byte2;
	return ret;
}

/*
get a printable character from a stream stream,and return value 
is in uint16_t form,return 0x00 03(byte sequence) if stream is null,
return 0x00 01 for eof,return 0x01 01while eof and fseek back error,
return 0x00 02 for invalid character, return 0x01 02 
for invalid character and fseek back error
*/
uint16_t fgetc_gbk_bin (FILE *stream)
{
	int byte1, byte2;
	byte1 = byte2 = 0;
	uint16_t ret;
	if (stream == NULL)
	{
		*(char *)&ret = 0x00;
		*((char *)&ret + 1) = 0x03;
		return ret; //return 0x00 03 if stream is null
	}
	if ((byte1 = fgetc (stream)) == EOF)
	{
		*(char *)&ret = 0;
		*((char *)&ret + 1) = 1;
		return ret; //return 0x00 01 for eof
	}
	if (byte1 <= 0x7F)
	{
		*(char *)&ret = (char)byte1;
		*((char *)&ret + 1) = 0;
		return ret;
	}
	if ((byte2 = fgetc (stream)) == EOF)
	{
		if (fseek (stream, (long) -1, SEEK_CUR))
		{
			return 0x0101; //for eof and fseek back error
		}
		*(char *)&ret = 0;
		*((char *)&ret + 1) = 1;
		return ret;
		//return 0x00 01 for eof
	}
	if ((unsigned)byte1 < 0x81 || (unsigned)byte1 > 0xFE || (unsigned)byte2 < 0x40 || (unsigned)byte2 > 0xFE)
	{
		if (fseek (stream, (long) -2, SEEK_CUR))
		{
			*(char *)&ret = 1;
			*((char *)&ret + 1) = 2;
			return ret; //return 0x01 02 for character invalid and fseek back error
		}
		*(char *)&ret = 0;
		*((char *)&ret + 1) = 2;
		return ret; // return 0x00 02 for character invalid
	}
	*(char *)&ret = byte1;
	*((char *)&ret + 1) = byte2;
	return ret;
}

/*
get a line of gbk string, return bytes read not including the terminating null,character_count is used to store 
the count of characters read, str is the string to be processed, bytes is the 
size of the str in bytes,return -2 if arg invalid, return -1 if invalid 
character,or unprintable character,or (eof and fseek back failed),a terminating 
null is at the end
*/
ssize_t fgets_gbk (char *str, size_t bytes, FILE* stream, size_t *characters_count)
{
	char *cur_pos;
	char *end = str + bytes; //position after last character in str
	uint16_t value;
	size_t char_count;
	if (stream == NULL || str == NULL || bytes <= 0) return -2; //arg invalid
	for (cur_pos = str, char_count = 0; cur_pos < end;)
	{
		if (cur_pos == end - 1)
		{
			*cur_pos = '\0';
			return cur_pos - str;
		}
		value = fgetc_gbk (stream);
		char *c = (char*)&value;
		if (c[0] == 0x00 && c[1] == 0x01) // EOF
		{
			*cur_pos = '\0';
			if (characters_count != NULL) *characters_count = char_count;
			return  cur_pos - str - 1; //EOF
		}

		if ((c[0] == 0x01 && c[1] == 0x01) || (c[0] == 0x01 && c[1] == 0x02) || (c[0] == 0x00 && c[1] == 0x02) || (c[0] == 0x02 && c[1] == 0x02))
		{
			return -1; //invalid character,or unprintable character,or (eof and fseek back failed)
		}

		if (c[0] <= 0x7F && c[0] > 0)
		{
			if (c[0] == '\n' && cur_pos != str && *(cur_pos - 1) == '\r') continue;
			else if (c[0] == '\r' || c[0] == '\n')
			{
				*cur_pos = '\n';
				if (characters_count != NULL) *characters_count = char_count + 1;
				cur_pos[1] = '\0';
				return cur_pos - str + 1;
			}
			*cur_pos = *(char*)&value;
			cur_pos++;
			char_count++;
			continue;
		}
		else if (cur_pos == end - 2)
		{
			if (fseek (stream, (long) -2, SEEK_CUR)) return (ssize_t) -1; //return indicate fseek error
			*cur_pos = '\0';
			if (characters_count != NULL) *characters_count = char_count;
			return cur_pos - str;
		}
		*cur_pos = *(char*)&value;
		*(cur_pos + 1) = *((char*)&value + 1);
		cur_pos += 2;
		char_count++;
	}
	if (characters_count != NULL) *characters_count = char_count;
	return cur_pos - str - 1;
}

/*
get a line of gbk string, return bytes read,character_count is used to store 
the count of characters read, str is the string to be processed, bytes is the 
size of the string in bytes,return -2 if stream is null, return -1 if invalid 
character,or unprintable character,or (eof and fseek back failed), a null 
terminating null is at the end of the str
*/
ssize_t fgets_gbk_bin (char *str, size_t bytes, FILE* stream, size_t *characters_count)
{
	char *cur_pos;
	char *end = str + bytes;
	uint16_t value;
	size_t char_count;
	if (stream == NULL) return -2; //stream is null
	for (cur_pos = str, char_count = 0; cur_pos < end;)
	{
		if (cur_pos == end - 1)
		{
			*cur_pos = '\0';
			*characters_count = char_count;
			return cur_pos - str;
		}
		value = fgetc_gbk_bin (stream);
		char *c = (char*)&value;
		if (c[0] == 0x00 && c[1] == 0x01) // EOF
		{
			*cur_pos = '\0';
			*characters_count = char_count;
			return  cur_pos - str; //EOF
		}

		if ((c[0] == 0x01 && c[1] == 0x01) || (c[0] == 0x01 && c[1] == 0x02) || (c[0] == 0x00 && c[1] == 0x02))
		{
			return -1; //invalid character, or (eof and fseek back failed)
		}

		if (c[0] <= 0x7F && c[0] > 0)
		{
			if (c[0] == '\n' && cur_pos != str && *(cur_pos - 1) == '\r') continue;
			if (c[0] == '\r' || c[0] == '\n')
			{
				*cur_pos = '\n';
				*characters_count = char_count + 1;
				cur_pos[1] = '\0';
				return cur_pos - str + 1;
			}
			*cur_pos = *(char*)&value;
			cur_pos++;
			char_count++;
			continue;
		}
		if (cur_pos == end - 2)
		{
			if (fseek (stream, (long) -2, SEEK_CUR)) return (ssize_t) -1; //return indicate fseek error
			*cur_pos = '\0';
			*characters_count = char_count;
			return cur_pos - str;
		}
		*cur_pos = *(char*)&value;
		*(cur_pos + 1) = *((char*)&value + 1);
		cur_pos += 2;
		char_count++;
	}
	*characters_count = char_count;
	return cur_pos - str - 1;
}

