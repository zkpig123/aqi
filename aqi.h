#include <stdlib.h>
#include <float.h>
#include <limits.h>
#include <inttypes.h>
#include <mysql.h>
#include <stdint.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>

#ifndef PATH_MAX
#define PATH_MAX 255
#endif

#define IP_ADDR_MAX 64
#define PORT_MAX 10
#define TYPE_NAME_LEN_MAX doas_so2_sig_noise_ratio
#define IAQI_LEN 4
#define DATA_ROUNDING_BIT1 3
#define DATA_ROUNDING_BIT2 1
#define RAW_TABLE_TEMPLATE "raw"
#define DATA_ACCURACY 0.0000001
#define DATA_ACCURACY_BIT 7
#define DATA_INTEGRAL_LEN 10
#define DATA_FRACTION_LEN 18
#define STRING_NUMBER_BUFSIZ DATA_INTEGRAL_LEN + DATA_FRACTION_LEN + 3 /*one for \0, one 
								for dot, one for sign*/
#define INTEGRAL_MAX_DECIMAL_BITS 9
#define FRACTION_MAX_DECIMAL_BITS 15

enum SITE {tyg = 0, tsg, invalid_site};
#define SITE_FIRST tyg
#define SITE_LAST tsg
char *get_site_name (enum SITE);
int check_site (enum SITE);

enum IAQI_TYPE {IAQI_SO2_1H, IAQI_SO2_24H, IAQI_NO2_1H, IAQI_NO2_24H, IAQI_O3_8H_MOVING, IAQI_O3_1H, IAQI_CO_1H, IAQI_CO_24H, IAQI_PM10_24H, IAQI_PM25_24H};
#define IAQI_TYPE_MIN IAQI_SO2_1H
#define IAQI_TYPE_MAX IAQI_PM25_24H
//return 0 if valid, other invalid
int check_iaqi_type (enum IAQI_TYPE iaqi_type);

/*#define IAQI_0_ALL 0
#define IAQI_50_SO2_1H 0.150
#define IAQI_100_SO2_1H 0.500
#define IAQI_150_SO2_1H 0.650
#define IAQI_200_SO2_1H 0.800
#define IAQI_50_SO2_24H 0.050
#define IAQI_100_SO2_24H 0.150
#define IAQI_150_SO2_24H 0.475
#define IAQI_200_SO2_24H 0.8
#define IAQI_300_SO2_24H 1.6
#define IAQI_400_SO2_24H 2.1
#define IAQI_500_SO2_24H 2.62
#define IAQI_50_NO2_24H 0.040
#define IAQI_100_NO2_24H 0.08
#define IAQI_150_NO2_24H 0.180
#define IAQI_200_NO2_24H 0.280
#define IAQI_300_NO2_24H 0.565
#define IAQI_400_NO2_24H 0.750
#define IAQI_500_NO2_24H 0.940
#define IAQI_50_NO2_1H 0.1
#define IAQI_100_NO2_1H 0.2
#define IAQI_150_NO2_1H 0.70
#define IAQI_200_NO2_1H 1.20
#define IAQI_300_NO2_1H 2.34
#define IAQI_400_NO2_1H 3.09
#define IAQI_500_NO2_1H 3.84
#define IAQI_50_PM10_24H 0.05
#define IAQI_100_PM10_24H 0.15
#define IAQI_150_PM10_24H 0.25
#define IAQI_200_PM10_24H 0.35
#define IAQI_300_PM10_24H 0.42
#define IAQI_400_PM10_24H 0.5
#define IAQI_500_PM10_24H 0.6
#define IAQI_50_PM25_24H 0.035
#define IAQI_100_PM25_24H 0.075
#define IAQI_150_PM25_24H 0.115
#define IAQI_200_PM25_24H 0.15
#define IAQI_300_PM25_24H 0.25
#define IAQI_400_PM25_24H 0.35
#define IAQI_500_PM25_24H 0.5
#define IAQI_50_CO_24H 2
#define IAQI_100_CO_24H 4
#define IAQI_150_CO_24H 14
#define IAQI_200_CO_24H 24
#define IAQI_300_CO_24H 36
#define IAQI_400_CO_24H 48
#define IAQI_500_CO_24H 60
#define IAQI_50_CO_1H 5
#define IAQI_100_CO_1H 10
#define IAQI_150_CO_1H 35
#define IAQI_200_CO_1H 60
#define IAQI_300_CO_1H 90
#define IAQI_400_CO_1H 120
#define IAQI_500_CO_1H 150
#define IAQI_50_O3_1H 0.16
#define IAQI_100_O3_1H 0.2
#define IAQI_150_O3_1H 0.3
#define IAQI_200_O3_1H 0.4
#define IAQI_300_O3_1H 0.8
#define IAQI_400_O3_1H 1
#define IAQI_500_O3_1H 1.2
#define IAQI_50_O3_8H 0.1
#define IAQI_100_O3_8H 0.16
#define IAQI_150_O3_8H 0.215
#define IAQI_200_O3_8H 0.265
#define IAQI_300_O3_8H 0.8*/

#define IAQI_0_ALL "0"
#define IAQI_50_SO2_1H "0.150"
#define IAQI_100_SO2_1H "0.500"
#define IAQI_150_SO2_1H "0.650"
#define IAQI_200_SO2_1H "0.800"
#define IAQI_50_SO2_24H "0.050"
#define IAQI_100_SO2_24H "0.150"
#define IAQI_150_SO2_24H "0.475"
#define IAQI_200_SO2_24H "0.8"
#define IAQI_300_SO2_24H "1.6"
#define IAQI_400_SO2_24H "2.1"
#define IAQI_500_SO2_24H "2.62"
#define IAQI_50_NO2_24H "0.040"
#define IAQI_100_NO2_24H "0.08"
#define IAQI_150_NO2_24H "0.180"
#define IAQI_200_NO2_24H "0.280"
#define IAQI_300_NO2_24H "0.565"
#define IAQI_400_NO2_24H "0.750"
#define IAQI_500_NO2_24H "0.940"
#define IAQI_50_NO2_1H "0.1"
#define IAQI_100_NO2_1H "0.2"
#define IAQI_150_NO2_1H "0.70"
#define IAQI_200_NO2_1H "1.20"
#define IAQI_300_NO2_1H "2.34"
#define IAQI_400_NO2_1H "3.09"
#define IAQI_500_NO2_1H "3.84"
#define IAQI_50_PM10_24H "0.05"
#define IAQI_100_PM10_24H "0.15"
#define IAQI_150_PM10_24H "0.25"
#define IAQI_200_PM10_24H "0.35"
#define IAQI_300_PM10_24H "0.42"
#define IAQI_400_PM10_24H "0.5"
#define IAQI_500_PM10_24H "0.6"
#define IAQI_50_PM25_24H "0.035"
#define IAQI_100_PM25_24H "0.075"
#define IAQI_150_PM25_24H "0.115"
#define IAQI_200_PM25_24H "0.15"
#define IAQI_300_PM25_24H "0.25"
#define IAQI_400_PM25_24H "0.35"
#define IAQI_500_PM25_24H "0.5"
#define IAQI_50_CO_24H "2"
#define IAQI_100_CO_24H "4"
#define IAQI_150_CO_24H "14"
#define IAQI_200_CO_24H "24"
#define IAQI_300_CO_24H "36"
#define IAQI_400_CO_24H "48"
#define IAQI_500_CO_24H "60"
#define IAQI_50_CO_1H "5"
#define IAQI_100_CO_1H "10"
#define IAQI_150_CO_1H "35"
#define IAQI_200_CO_1H "60"
#define IAQI_300_CO_1H "90"
#define IAQI_400_CO_1H "120"
#define IAQI_500_CO_1H "150"
#define IAQI_50_O3_1H "0.16"
#define IAQI_100_O3_1H "0.2"
#define IAQI_150_O3_1H "0.3"
#define IAQI_200_O3_1H "0.4"
#define IAQI_300_O3_1H "0.8"
#define IAQI_400_O3_1H "1"
#define IAQI_500_O3_1H "1.2"
#define IAQI_50_O3_8H "0.1"
#define IAQI_100_O3_8H "0.16"
#define IAQI_150_O3_8H "0.215"
#define IAQI_200_O3_8H "0.265"
#define IAQI_300_O3_8H "0.8"

#define O3_8H_MOVING_AVERAGE_LIMIT 0.2
#define RESERVED_STRING_LEN 20
#define TYG "tyg"
#define TSG "tsg"
#define SITENAMEMAX 5 //not include terminating null
#define AQIDATABASE "aqi"
#define RAWFILE "out.txt"
#define RAW_TABLE_NAME_PREFIX "raw_"
#define HOUR_AVERAGE_TABLE_NAME_PREFIX "hour_average_"
#define O3_1H_MAX_TABLE_NAME_SUFFIX "_O3_1h_max"
#define O3_8H_MOVING_AVERAGE_TABLE_NAME_PREFIX "O3_8h_moving_average_"
#define RESERVED_LEN 20
#define O3_8H_MOVING_AVERAGE_MAX_TABLE_NAME_SUFFIX "_O3_8h_moving_average_max"
#define DAY_AVERAGE_TABLE_NAME_SUFFIX "_day_average"

#ifdef UNIT
#if UNIT != MGM3
#define UNIT MGM3
#endif
#else
#define UNIT MGM3
#endif

//type of audit, invalid_audit_type is returned to indicate that audit is not correct audit_type
enum AUDIT_TYPE {B, BB, W, H, HSp, LSp, PZ, PS, AS, CZ, CS, Re, T, empty_audit, invalid_audit_type};
#define AUDIT_TYPE_ENUM_MIN B
#define AUDIT_TYPE_ENUM_MAX T

//type of monitor type, invalid_type is returned to indicate that type is not correct type
enum TYPE {SO2 = 0, NO2, O3, CO, PM10, PM25, O3_8h, O3_8h_max, O3_1h_max, wind_velocity, ambient_temperature, humidity, atom_pressure, wind_direction, room_temperature, doas_light_insensity, doas_monitor_status, doas_integral_time, doas_average_count, doas_optical_path, doas_so2_sig_noise_ratio, doas_no2_sig_noise_ratio, doas_o3_sig_noise_ratio, doas_sig_noise_ratio, doas_pda_status, doas_elec_machine_status, doas_adjust_poll_pos, doas_mercury_lamp_pos, doas_mercury_lamp_power, invalid_type};
#define TYPE_ENUM_MAX doas_mercury_lamp_power
#define TYPE_ENUM_MIN SO2

#define SO2_MAX 500*64/22.4/1000
#define NO2_MAX 500*46/22.4/1000
#define O3_MAX 500*48/22.4/1000
#define CO_MAX 50*30/22.4

/*
 * get upper limit value of given type,return -1 if error,
 * is now only following type has a upper limit in machine,
 * SO2,NO2,O3,CO
*/
double get_type_max (enum TYPE type);

char *get_audit_name (enum AUDIT_TYPE type);

char *get_audit_chinese (enum AUDIT_TYPE audit);

//return 0 if valid, other if invalid,
//from_mysql indicate type string is result of mysql query,0 if not,
//other yes
int check_audit_string (char *type, int from_mysql);

//return 0 if valid, other if invalid
int check_audit_type (enum AUDIT_TYPE type);

enum AUDIT_TYPE get_audit_type (char *type, int from_mysql);

enum log_type {import, first_stat, audit, second_stat};
#define IMPORTLOG "import"
#define FIRSTSTATLOG "stat1"
#define AUDITLOG "audit"
#define SECONDSTATLOG "stat2"
#define LOGENUMMIN import
#define LOGENUMMAX second_stat

/*
 * generate a file name corresponding to given arg for log file is current date and time with a 
 * format "YYYY-MM-DD-HHMMSS-type.log" , return buffer using malloc or null if failure, caller
 * should free buffer after using this buffer if success, return NULL if failure,
*/
char* log_file_name (enum log_type log);

/*default hostname, user, pw, db,clientcharset,serverport for create table for mysql from client*/
#define HOST "localhost"
#define MYSQLUSER "zk"
#define PASSWORD "kaige"
#define DATABASE "aqi"
#define CLIENTCHARSET "gbk"
#define MYSQLPORT 3306
#define QUERYEXISTLEN sizeof ("SHOW TABLES LIKE 'raw_YYYYMMDD'") - 1
#define RAWTABLENAMELEN sizeof ("raw_YYYYMMDD") - 1
#define HOURAVERAGETABLENAMELEN sizeof ("hour_average_YYYYMM") - 1
#define DAYAVERAGETABLENAMELEN sizeof ("day_average") - 1

#define MAXLINE 100
#define TIMELEN 19 //19 + 1 "YYYY-MM-DD HH:MM:SS"
#define TYPELEN 16 
#define VALUELEN 25
#define INTEGRAL 12
#define FRACTION 12
#define FLAGLEN 3
#define LOGFILENAMELEN 22
#define MINDATALEN 20 //min len of data line

//get name of gas type according to arg type whose type is enum TYPE,or NULL if type arg invalid
char* get_type_name (enum TYPE type);

/*
 * return max len of type name of all enum type
 */
unsigned int get_type_name_len_max (void);

//check date formatted as yyyymmdd,return 0 if valid, other invalid
int check_date (uint32_t date);

//check time formated as hhmmss, return 0 if valid, other invalid
int check_time (uint32_t time);

//check type, return 0 if valid, other invalid
int check_type_enum (enum TYPE type);

/*
 * get enum TYPE value of given string name, return corresponding 
 * enum TYPE or invalid_type if no matching type found
*/
enum TYPE get_enum_type (char *name);

int check_string_of_value (char *value);

/*
 * check time designated "YYYY-MM-DD HH:MM:SS", return 0 if 
 * formatted right, or other if invalid
*/
int check_string_of_time (char *time);

/*
 * check time designated "YYYY-MM-DD HH:MM:SS", return 0 if 
 * formatted right, or other if invalid
*/
int check_string_of_time_from_mysql (char *time);

//get a string used in mysql table creat sentence which require a enum type of the type of gas,
//for function creat_table_raw that create table of raw data
//like create table xxxx (enum ('so2','no2',.......'
//we want the string 'so2','no2','o3','co'............
//return pointer created using malloc to that string if success, caller should free it !
//return NULL if failed,
char *mysql_enum_type_string (void);

/*
 * get a string used in mysql table creat 
 * sentence which require a enum type of 
 * the type of audit type for function 
 * creat_table_hour_average that create table 
 * of hour average like create table xxxx 
 * (enum ('B','BB'.......' , here we wanna the 
 * string 'B','BB',............
 * return pointer created using malloc to that 
 * string if success, caller should free it ! 
 * return NULL if failed,
*/
char *mysql_enum_audit_type_string (void);

/*check existence of table named "raw_YYYYMMDD" in mysql database corresponding to arg,mysql is connection established to mysql, can be null if not supplied,if so established one using predefined macro of servername,port,username,password,database in including file
return 1 if exist, or 0 not exist, or other if error occur
*/
int check_raw_table_exist (int year, int month, int day, char *site, MYSQL *mysql);

/*
 * create table of raw data with given arg, return 0 if ok, return other if error occur,mysql is connection having established to mysql server, or null and this function will establish one, using predefined macros of server,port,username,password,database,client charset
*/
int create_table_raw (uint16_t year, uint16_t month, uint16_t day, enum SITE sitetype, MYSQL *mysql);

/*create table of hour average data with given arg, return 0 if ok, return other if error occur,mysql is 
 * connection having established to mysql server,
*/
int create_table_hour_average (uint16_t year, uint16_t month, enum SITE sitetype, MYSQL *mysql);

int check_year_month_day (uint16_t year, uint16_t month, uint16_t day);

int check_hour_minute_second (uint16_t hour, uint16_t minute, uint16_t second);

/*establish a connect using default server,
 * port,user,password,database,charset,return 
 * MYSQL connection if ok or null if error
*/
MYSQL* get_mysql_connection (void);

/*establish a connect using default server with
 * multi statement supporting
 * port,user,password,database,charset,return 
 * MYSQL connection if ok or null if error
*/
MYSQL* get_mysql_connection_multi_statement (void);

/*establish a connect using given arg,
 * port,user,password,database,charset,return 
 * MYSQL connection if ok or null if error
*/
MYSQL* get_mysql_connection_arg (char *server, char *mysql_user, char *password, char *database, unsigned int port, char *client_charset);

//redirect given stream to file,return 0 if success,other failure
int redirect_stream_to_file (FILE *fp, char *filename);

/*
 * get last day of today, returned as unsigned int 
 * as YYYYMMDD or 0 if arg invalid
*/
unsigned int get_last_day (uint16_t year, uint16_t month, uint16_t day);

//return string of month formatted "MM"
char * get_month_string (uint16_t month);

/*
 * get upper limit value of given type,return -1 if error,
 * is now only following type has a upper limit in machine,
 * SO2,NO2,O3,CO
*/
double get_type_value_max (enum TYPE type);

/*create table of day average data with given arg, return 0 if ok, return other if error occur,mysql is 
 * connection having established to mysql server, site is name of place,tyg or tsg
*/
int create_table_day_average (enum SITE sitetype, MYSQL *mysql);

/*
 * get max_allowed_packet of mysql connection
 * using sql query show variable like...,return 
 * correpsonding value, return 0 indicate
 * error
 */
size_t get_mysql_connection_max_allowed_packet_bytes (MYSQL *mysql);

/*create table of O3 8h moving average data with given arg, return 0 if ok, return other if error occur,mysql is 
 * connection having established to mysql server, site is name of place,tyg or tsg
*/
int create_table_O3_8h_moving_average (uint16_t year, uint16_t month, enum SITE sitetype, MYSQL *mysql);

//return corresponding hour of last hours or 25 indicationg given arg cur_hour invalid
unsigned int get_hour_of_x_hours_ago (uint16_t cur_hour, uint16_t hours);

int create_table_O3_1h_max (enum SITE sitetype, MYSQL *mysql);

/*
 * create table of O3 8h moving average max of day,
 * return 0 if ok, return other if error occur
 */
int create_table_O3_8h_moving_average_max (enum SITE sitetype, MYSQL *mysql);

/*
 * calc IAQI of 1h average of SO2, return corresponding
 * value or -1 indicating invalid value, return -2
 * indicate that if value greater than 800, then IAQI of 1h
 * average of so2 is the same as 24h average of so2
 */
int16_t calc_IAQI_SO2_1h (double value);

/*
 * calc IAQI of 24h average of SO2, return corresponding
 * value or -1 indicating invalid value, 
 */
int16_t calc_IAQI_SO2_24h (double value);

/*
 * check iaqi type,return 0 if valid ,other invalid
 */
int check_aqi (int16_t aqi);

/*
 * return iaqi of given iaqi_type and value,return -2 if type is 
 * 1h average of so2 and value greater than value
 * of iaqi boundary 200, or type is 8h moving average of o3 and value 
 * greater than value of iaqi boundary 300 or value is -1, return -1 
 * if value invalid, return -3 if unknown err,
 */
int calc_IAQI (enum IAQI_TYPE iaqi_type, char* value);

/*
 * check whether value and audit is matched,
 * return -1 if audit invalid, return 1 if not matched,
 * return 0 if matched
 */
int check_value_audit_match (char *value, enum AUDIT_TYPE audit);

/*
 * construct mysql lock query of hour average
 */
char *construct_hour_average_table_lock_mysql_query (uint16_t year, uint16_t month, enum SITE sitetype);

/*
 * return rounding data of value that was monitoring
 * under normal condition and is less than zero,returning
 * rounded result or remaining unchangable if value is greater
 * than 0,return  -1 indicating invalid cause less than a 
 * specific value, return -2 if type invalid
 */
int rounding_data (char *value, enum TYPE type);

/*
 * construct mysql lock query of hour average
 */
char *construct_hour_average_table_lock_mysql_query (uint16_t year, uint16_t month, enum SITE sitetype);

char *construct_O3_moving_average_max_table_lock_mysql_query (enum SITE sitetype);

/*
 * construct mysql lock query of O3 8h
 * moving average
 */
char *construct_O3_moving_average_table_lock_mysql_query (enum SITE sitetype, uint16_t year, uint16_t month);

/*
 * construct mysql lock query of O3 1h max
 */
char *construct_day_average_table_lock_mysql_query (enum SITE sitetype);

enum SITE get_site_enum (char *site);

/*
 * stat hour average from raw data, return 0 if ok, return 3
 * if data invalid, return 2 if data more than one data 
 * exist(PM10,PM25), return 1 if no data exist(PM10,PM25),return -1 
 * if arg invalid, return other negative integer if some error occur,
 * other if failed
 */
int stat_hour_average (uint16_t year, uint16_t month, uint16_t day, uint16_t hour, enum TYPE type, char *reserved_string, enum SITE sitetype, MYSQL *mysql);

/*
 * construct mysql lock query of O3 1h max
 */
char *construct_O3_1h_max_table_lock_mysql_query (enum SITE sitetype);

int check_YYYYMMDD_string (char *s);

/*
 * rounding a floating point,奇进偶不进, position 
 * indicate position of rounding at the right direction
 * of the decimal point
 */
int our_rounding (double *value, unsigned int position);

/*
 * rounding a floating point,奇进偶不进, position 
 * indicate position of rounding at the right direction
 * of the decimal point
 */
int our_string_rounding (char *num, unsigned bytes, unsigned int position);

/*
 * get tomorrow of given day, returned as uint32_t
 * as YYYYMMDD or 0 if arg invalid
*/
uint32_t get_tomorrow (uint16_t year, uint16_t month, uint16_t day);

/*
 * check number in string, return 0 if valid, return other
 * if invalid,number is from mysql database formatted 
 * decimal (18,9), meaning has at most 9 integral part and 9 
 * fractional part
 */
int check_string_number_of_value (char *num);

/*
 * add numbers(should be positive) in two string, number is retrieved from
 * mysql database formatted decimal (18.9), so length of
 * number part of two number should less than 19 or 20(negative)
 * ,length of sum should be at least 26,return 0 if ok, return 1 if overflow,
 */
int add_string_number (char *n1, char *n2, char *sum, unsigned int bytes);

long long our_atoll (char *s, unsigned int count);

/*
 * divide must be an integer
 */
int divide_string_number (char *divisor, int32_t divide, unsigned int accuracy, char *quotient, unsigned bytes);

int eliminate_fraction_string_trailing_zero (char *s);

/*
 * compare two string number, return 0 if equal, return
 * 1 if value1 greater than value2, return -1 if less,
 * return -INT_MAX if arg invalid
 */
int compare_string_number (char *value1, char *value2);

int multiply_string_number (char *value1, char *value2, char *mul, unsigned bytes);

char * get_value_of_IAQI_BOUNDARY (enum IAQI_TYPE iaqi_type, int16_t iaqi);

//minuend and substrahend must be positive,return 0 if ok,
int substract_string_number (char *v1, char *v2, char *sub, unsigned bytes);

int divide_string_number_char (char *divisor, char *divide, unsigned int accuracy, char *quotient, unsigned bytes);

/*
 * get opposite number, return 0 if ok, return 1 if bytes
 * not enough to hold '-', return -1 if arg invalid
 */
int negative_string_number (char *num, unsigned bytes);

/*
 * get O3 8h from hour average data, return 0 
 * if successfully get an normal result,return 1
 * if valid hours less than 6,return 2 if morethan
 * 8 result of hour average value exist,return 3 if exist
 * invalid hour average value, return 4 if insert failed,
 * return -1 if arg invalid, return other negative integer 
 * if some err occur
 */
int stat_O3_8h_moving_average(uint16_t year, uint16_t month, uint16_t day, uint16_t hour, enum SITE sitetype, MYSQL *mysql);

/*
 * get O3 8h moving average max from moving average table and
 * get the max, return 0 if successfully get normal value, return 
 * 2 if invalid moving average exist and value is set, return 3 if 
 * count of moving average greater than 17,return 4 if get some 
 * moving average failed,return 5 if create o3 8h moving average 
 * max table failed, return -1 if arg invalid, return other 
 * negative integer if other err occur
 */
int stat_O3_8h_moving_average_max (uint16_t year, uint16_t month, uint16_t day, enum SITE sitetype, MYSQL *mysql);

/*
 * stat O3 1h max return 0 if query o3 1h max table 
 * success,return 3 if some hour average invalid,
 * return  4 if create table failed, return 2 if get 
 * some hour average failed,return 1 if no value exist
 * in hour average data, return -1 if arg invalid, return other
 * negative interger if other err occur, return 5 if more
 * than 24 hour average value exist
 */

int stat_O3_1h_max (uint16_t year, uint16_t month, uint16_t day, enum SITE sitetype, MYSQL *mysql);
/*
 * stat day average value and data,and insert into day
 * average table,return 3 if some hour average invalid,return
 * 2 if get some hour average failed,return 1 if valid
 * hours less than 20, return -1 if arg invalid, return other
 * negative interger if other err occur
 */
int stat_day_average (uint16_t year, uint16_t month, uint16_t day, enum TYPE type, char *reserved_string, enum SITE sitetype, MYSQL *mysql);

/*
 * audit hour average value, return 0 if successfully replace original
 * data, return 1 if there is originally data in the hour average
 * table,return -1 if arg invalid, return other negative value if
 * other err occur
 */
int audit_hour_average (uint16_t year, uint16_t month, uint16_t day, uint16_t hour, enum TYPE type, enum AUDIT_TYPE audit, char *reserved_string, enum SITE sitetype, MYSQL *mysql);

struct tm* get_time (void);
