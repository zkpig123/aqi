#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>

//character count of gbk string not include terminating null, unprintable character is invalid, bytes not including the terminating null,return -1 if arg invalid, return -2 if character invalid or unprintable character
ssize_t char_count_gbk (char *str, size_t bytes);

//character count of gbk string not including terminating null, unprintable character is valid, bytes not including the terminating null,return -1 if arg invalid, return -2 if character invalid
ssize_t char_count_gbk_bin (char *str, size_t bytes);

//byte count of gbk string non including terminating nul,bytes not including the terminating null return -1 if arg invalid, return -2 if character invalid or unprintable
ssize_t byte_count_gbk (char *str, size_t bytes);

//byte count of gbk string not including terminating null,bytes not including the terminating null, return -1 if arg invalid, return -2 if character invalid
ssize_t byte_count_gbk_bin (char *str, size_t bytes);

//check whether encoding of specified gbk string is valid, bytes not including the terminating null,return 0 if valid, return 1 if containing unprintable character,invalid when return other value
int char_check_gbk (char *str, size_t bytes);

/*
return first occurency position of target character in str,position is 
count in character but not byte,first position is 1 not 0, and validate 
check is performed for target but not str, return 0 if not exist, return 
-1 if arg invalid, return -2 if target is has invalid character encoding 
bytes not including the terminating null
*/
ssize_t char_pos_gbk (char *str, size_t bytes1, char *target, size_t bytes2);

/*
 * return first occurency position of target character in str,position is count 
 * in byte,first byte is 0 not 1, and validate check is performed for target but not str 
 * return -1 if arg invalid, return -2 if target has invalid encoding, return -3
 * if not exist
*/
ssize_t byte_pos_gbk (char *str, size_t bytes1, char *target, size_t bytes2);

/*
get a printable character from a stream stream,and return value 
is in uint16_t form,return 0x00 03(byte sequence) if stream is null,
return 0x00 01 for eof, return 0x02 02 for unprintable character,return 0x01 01
while eof and fseek back error,return 0x00 02 for invalid character, return 0x01 02 
for invalid character and fseek back error
*/
uint16_t fgetc_gbk (FILE *stream);

/*
get a printable character from a stream stream,and return value 
is in uint16_t form,return 0x00 03(byte sequence) if stream is null,
return 0x00 01 for eof,return 0x01 01while eof and fseek back error,
return 0x00 02 for invalid character, return 0x01 02 
for invalid character and fseek back error
*/
uint16_t fgetc_gbk_bin (FILE *stream);

/*
get a line of gbk string, return bytes read not including the terminating null,character_count is used to store 
the count of characters read, str is the string to be processed, bytes is the 
size of the str in bytes,return -2 if arg invalid, return -1 if invalid 
character,or unprintable character,or (eof and fseek back failed),a terminating 
null is at the end
*/
ssize_t fgets_gbk (char *str, size_t bytes, FILE* stream, size_t *characters_count);

/*
get a line of gbk string, return bytes read,character_count is used to store 
the count of characters read, str is the string to be processed, bytes is the 
size of the string in bytes,return -2 if stream is null, return -1 if invalid 
character,or unprintable character,or (eof and fseek back failed), a null 
terminating null is at the end of the str
*/
ssize_t fgets_gbk_bin (char *str, size_t bytes, FILE* stream, size_t *characters_count);
