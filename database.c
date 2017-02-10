#include "aqi.h"
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

/*
 * establish a connect using default server,
 * port,user,password,database,charset,return 
 * MYSQL connection if ok or null if error
*/
MYSQL *get_mysql_connection (void)
{
	MYSQL *mysql = NULL;

	if ((mysql = mysql_init(NULL)) == NULL) return NULL; //return NULL if error
	if (mysql_options (mysql, MYSQL_SET_CHARSET_NAME, CLIENTCHARSET))
	{
		mysql_close (mysql);
		return NULL; //return NULL if error
	}
	if (mysql_real_connect (mysql, HOST, MYSQLUSER, PASSWORD, DATABASE, MYSQLPORT, NULL, 0) == NULL)
	{
		mysql_close (mysql);
		return NULL; //return NULL if error
	}
	return mysql;
}

//connection with multi statement
MYSQL *get_mysql_connection_multi_statement (void)
{
	MYSQL *mysql = NULL;

	if ((mysql = mysql_init(NULL)) == NULL) return NULL; //return NULL if error
	if (mysql_options (mysql, MYSQL_SET_CHARSET_NAME, CLIENTCHARSET))
	{
		mysql_close (mysql);
		return NULL; //return NULL if error
	}
	if (mysql_real_connect (mysql, HOST, MYSQLUSER, PASSWORD, DATABASE, MYSQLPORT, NULL, CLIENT_MULTI_STATEMENTS | CLIENT_MULTI_RESULTS) == NULL)
	{
		mysql_close (mysql);
		return NULL; //return NULL if error
	}
	return mysql;
}

/*establish a connect using given arg,
 * port,user,password,database,charset,return 
 * MYSQL connection if ok or null if error
*/
MYSQL* get_mysql_connection_arg (char *server, char *mysql_user,
	       	char *password, char *database, unsigned int port, char *client_charset)
{
	MYSQL *mysql = NULL;

	//check arg
	if (server == NULL || mysql_user == NULL 
	|| password == NULL || database == NULL
	|| port < 1024 || client_charset == NULL) return NULL;
	if ((mysql = mysql_init(NULL)) == NULL) return NULL; //return NULL if error
	if (mysql_options (mysql, MYSQL_SET_CHARSET_NAME, client_charset))
	{
		mysql_close (mysql);
		return NULL; //return NULL if error
	}
	if (mysql_real_connect (mysql, server, mysql_user, password, database, port, NULL, 0) == NULL)
	{
		mysql_close (mysql);
		return NULL; //return NULL if error
	}
	return mysql;
}

/*
 * get max_allowed_packet of mysql connection
 * using sql query show variable like...,return 
 * correpsonding value, return 0 indicate
 * error
 */
size_t get_mysql_connection_max_allowed_packet_bytes (MYSQL *mysql)
{ 
	if (mysql == NULL)
	{
		fprintf (stderr, "get_mysql_connection_max_allowed_packet err:mysql is null");
		return 0;
	}
#ifdef MYSQL_OPT_MAX_ALLOWED_PACKET
	unsigned long packet;
	if (mysql_get_option (mysql, MYSQL_OPT_MAX_ALLOWED_PACKET, &packet))
	{
		fprintf (stderr, "mysql_get_option for max_allowed_packet err\n,mysql errno:%u,err:%s", mysql_errno (mysql), mysql_error (mysql));
		return 0;
	}
	return (size_t) packet;
#else
	char *get = "show variables like 'max_allowed_packet'";
	if (mysql_real_query (mysql, get, strlen (get)))
	{
		fprintf (stderr, "mysql_real_query for max_allowed_packet err, errno:%u,err:%s\n", mysql_errno (mysql), mysql_error (mysql));
		return 0;
	}
	MYSQL_RES *result;
	if ((result = mysql_store_result (mysql)) == NULL)
	{
		fprintf (stderr, "mysql_store_result for max_allowed_packet err,errno:%u,err:%s\n", mysql_errno (mysql), mysql_error (mysql));
		return 0;
	}
	MYSQL_ROW row;
	my_ulonglong rows;
	if ((rows = mysql_num_rows(result)) != 1)
	{
		fprintf (stderr, "mysql_num_rows not return 1 for max_allowed_packet\n");
		return 0;
	}
	if ((row = mysql_fetch_row (result)) == NULL)
	{
		fprintf (stderr, "mysql_fetch_row for max_allowed_packet err,errno:%u,err:%s\n", mysql_errno (mysql), mysql_error (mysql));
		return 0;
	}
	unsigned int fields, sn;
	fields = mysql_num_fields (result);
	sn = fields - 1;
	unsigned long *lengths;
	if ((lengths = mysql_fetch_lengths (result)) == NULL)
	{
		fprintf (stderr, "mysql_fetch_lengths for max_allowed_packet err\n,errno:%u,err:%s", mysql_errno (mysql), mysql_error (mysql));
		return 0;
	}
	if (row[sn] == NULL)
	{
		fprintf (stderr, "null row of max_allowed_packet column\n");
		return 0;
	}
	size_t size = 0;
	for (unsigned int i = 0; i < lengths[sn]; i++)
	{
		if (!isdigit (row[sn][i]))
		{
			fprintf (stderr, "invalid character in result string of max_allowed_packet column value\n");
			return 0;
		}
		else size = size * 10 + row[sn][i] - '0';
	}

	if (size == 0)
	{
		fprintf (stderr, "fetched value for max_packet_length failed\n");
		return 0;
	}
	else return size;
#endif
}

//get a string used in mysql table creat sentence which require a enum type of the type of gas,
//for function creat_table_raw that create table of raw data
//like create table xxxx (enum ('so2','no2',.......'
//we want the string 'so2','no2','o3','co'............
//return pointer created using malloc to that string if success, caller should free it !
//return NULL if failed,
char *mysql_enum_type_string (void)
{
	char *str = NULL;
	size_t len = 0;
	size_t total_len = 1;
	for (int i = TYPE_ENUM_MIN; i <= TYPE_ENUM_MAX; i++)
	{
		char *item = get_type_name (i);
		int tmp = total_len - 1;
		len = strlen (item);
		total_len += len + 2;
		if (i != TYPE_ENUM_MIN) total_len++;
		if ((str = realloc (str, total_len)) == NULL) return NULL;
		if (i != TYPE_ENUM_MIN)
		{
			str[tmp] = ',';
			tmp++;
		}
		str[tmp] = '\'';
		strncpy (str + tmp + 1, item, len);
		str[total_len - 2] = '\'';
	}
	str[total_len - 1] = '\0';
	return str;
}

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
char *mysql_enum_audit_type_string (void)
{
	char *str = NULL;
	size_t len = 0;
	size_t total_len = 1;

	//here AUDIT_TYPE_ENUM_MAX + 1 contain the empty audit type
	for (int i = AUDIT_TYPE_ENUM_MIN; i <= AUDIT_TYPE_ENUM_MAX + 1; i++)
	{
		char *item = get_audit_name (i);
		int tmp = total_len - 1;
		len = strlen (item);
		total_len += len + 2;
		if (i != AUDIT_TYPE_ENUM_MIN) total_len++;
		if ((str = realloc (str, total_len)) == NULL) return NULL;
		if (i != AUDIT_TYPE_ENUM_MIN)
		{
			str[tmp] = ',';
			tmp++;
		}
		str[tmp] = '\'';
		strncpy (str + tmp + 1, item, len);
		str[total_len - 2] = '\'';
	}
	str[total_len - 1] = '\0';
	return str;
}

/*create table of raw data with given arg, return 0 if ok, return other if error occur,mysql is connection having established to mysql server,
*/
int create_table_raw (uint16_t year, uint16_t month, uint16_t day, enum SITE sitetype, MYSQL *mysql)
{
	char *enum_string;
	char *query = NULL;
	MYSQL_ROW mysql_row;
	char *site;
	int ret;
	
	//check arg
	if (mysql == NULL || check_year_month_day (year, month, day) || check_site (sitetype)) return 1; //arg invalid
	if ((site = get_site_name (sitetype)) == NULL) return 2;
	
	//construct the insert query
	if ((enum_string = mysql_enum_type_string()) == NULL) return 2; //return 2 if get enum string fail
	int query_len = strlen ("CREATE TABLE IF NOT EXISTS ") + strlen (site) + strlen ("_") + strlen (RAW_TABLE_NAME_PREFIX) + strlen ("YYYYMMDD (\
time DATETIME NOT NULL,\
type enum (") + strlen (enum_string) + strlen (") NOT NULL,\
value decimal(28,18) NOT NULL,\
flag varchar(3) DEFAULT NULL,\
PRIMARY KEY (time, type)) ENGINE=InnoDB DEFAULT CHARSET=gbk");
	
	if ((query = malloc (query_len)) == NULL)
	{
		free (enum_string);
		return 3; //return 3 if malloc failed
	}
	if ((ret = snprintf (query, query_len, "CREATE TABLE IF NOT EXISTS %s_%s%4" PRIu16 "%02" PRIu16 "%02" PRIu16 " \
(time DATETIME NOT NULL,\
type enum(%s) NOT NULL,\
value decimal(28,18) NOT NULL,\
flag varchar(3) DEFAULT NULL,\
PRIMARY KEY (time, type)) ENGINE=InnoDB DEFAULT CHARSET=gbk",
site, RAW_TABLE_NAME_PREFIX, year, month, day, enum_string)) < 0)
	{
		free (query);
		free (enum_string);
		return 4; //return 4 if sprintf failure
	}
	if ((size_t)ret + 1 > query_len)
	{
		free (query);
		free (enum_string);
		return 4; //return 4 if sprintf failure
	}
	free (enum_string);

	if (mysql_query (mysql, query))
	{
		free (query);
		return 5; //return 5 if mysql_query for exist of table failure
	}

	free (query);
	return 0;
}

/*create table of hour average data with given arg, return 0 if ok, return other if error occur,mysql is 
 * connection having established to mysql server,
*/
int create_table_hour_average (uint16_t year, uint16_t month, enum SITE sitetype, MYSQL *mysql)
{
	char *enum_string, *audit_enum_string;
	char *month_string;
	char *query = NULL;
	MYSQL_ROW mysql_row;
	char table_name[HOURAVERAGETABLENAMELEN + 1];
	int ret;
	char *site;
	
	//check arg
	if (mysql == NULL || check_year_month_day (year, month, 0) || check_site (sitetype)) return 1; //arg invalid
	if ((site = get_site_name (sitetype)) == NULL) return 1;
	
	//construct the insert query
	if ((enum_string = mysql_enum_type_string()) == NULL) return 2; //return 2 if get enum string fail
	if ((audit_enum_string = mysql_enum_audit_type_string()) == NULL)
	{
		free (enum_string);
		return 2; //return 2 if get enum string fail
	}
	month_string = get_month_string (month);
	int query_len = strlen ("CREATE TABLE IF NOT EXISTS ") + strlen (site) + strlen ("_") + strlen (HOUR_AVERAGE_TABLE_NAME_PREFIX) + strlen ("YYYYMM (\
day DATE NOT NULL,\
hour tinyint NOT NULL,\
type enum(") + strlen (enum_string) + strlen (") NOT NULL,\
value decimal(28,18) NOT NULL,\
audit_type enum(") + strlen (audit_enum_string) + strlen (") DEFAULT '',\
aqi smallint DEFAULT NULL,\
reserved varchar(20) DEFAULT '',\
PRIMARY KEY (day,hour,type)) ENGINE=InnoDB DEFAULT CHARSET=gbk") + 1;
	
	if ((query = malloc (query_len)) == NULL)
	{
		free (audit_enum_string);
		free (enum_string);
		return 3; //return 3 if malloc failed
	}
	if ((ret = snprintf (query, query_len, "CREATE TABLE IF NOT EXISTS %s_%s%4" PRIu16 "%s (\
day DATE NOT NULL,\
hour tinyint NOT NULL,\
type enum(%s) NOT NULL,\
value decimal(28,18) NOT NULL,\
audit_type enum(%s) DEFAULT '',\
aqi smallint DEFAULT NULL,\
reserved varchar(20) DEFAULT '',\
PRIMARY KEY (day,hour,type)) ENGINE=InnoDB DEFAULT CHARSET=gbk", 
		site, HOUR_AVERAGE_TABLE_NAME_PREFIX, year, month_string, enum_string, audit_enum_string)) < 0)
	{
		free (enum_string);
		free (audit_enum_string);
		free (query);
		return 4; //return 4 if sprintf failure
	}
	if ((size_t)ret + 1 > query_len)
	{
		free (enum_string);
		free (audit_enum_string);
		free (query);
		return 4; //return 4 if sprintf failure
	}
	free (enum_string);
	free (audit_enum_string);

	if (mysql_query (mysql, query))
	{
		fprintf (stderr, "create hour average table failed,query string : %s\n", query);
		free (query);
		return 5; //return 5 if mysql_query for exist of table failure
	}

	free (query);
	return 0;
}

/*check existence of table named "raw_YYYYMMDD" in mysql database corresponding to arg,
**mysql is connection established to mysql, can be null if not supplied,if so established 
one using predefined macro of servername,port,username,password,database in including file
**return 1 if exist, or 0 not exist, or other if error occur
*/
int check_raw_table_exist (int year, int month, int day, char *site, MYSQL *mysql)
{
	MYSQL_RES *mysql_res = NULL;
	MYSQL_ROW mysql_row;
	int exist_table = 0;
	size_t query_len, name_len;
	char *query, *name;
	int ret;

	//check arg
	if (check_year_month_day (year, month, day) || mysql == NULL || site == NULL) return 1; //arg invalid
	query_len = strlen ("SHOW TABLES LIKE '") + strlen (site) + strlen ("_") + strlen (RAW_TABLE_NAME_PREFIX) + strlen ("YYYYMMDD") + 1;
	name_len = strlen (site) + strlen ("_raw_YYYYMMDD") + 1;
	if ((query = malloc (query_len)) == NULL || (name = malloc (name_len)) == NULL) return 2;

	//construct query string
	if ((ret = snprintf (query, query_len, "SHOW TABLES LIKE '%s_%s%4" PRIu16 "%02" PRIu16 "%02" PRIu16 "'", site, RAW_TABLE_NAME_PREFIX, year, month, day)) < 0) return 5; //return 5 if sprintf failure
	else if ((size_t)ret + 1 > query_len) return 5;
	if ((ret = snprintf (name, name_len, "%s_%s%4" PRIu16 "%02" PRIu16 "%02" PRIu16, site, RAW_TABLE_NAME_PREFIX, year, month, day)) < 0) return 6; //return 6 if snprintf failure for table name
	else if ((size_t)ret + 1 > query_len) return 5;

	//query and process
	if (mysql_query (mysql, query)) return 7; //return 7 if mysql_query for exist of table failure
	if ((mysql_res = mysql_store_result (mysql)) == NULL) return 8; //return 8 if mysql_store_result for exist of table failure
	for (;;)
	{
		unsigned long *lengths;
		unsigned int num_fields;

		if (mysql_row = mysql_fetch_row (mysql_res)) break;
		if ((lengths = mysql_fetch_lengths (mysql_res)) == NULL)
		{
			mysql_free_result (mysql_res);
			return 9; //return 9 if mysql_fetch_lengths for exist of table failure
		}
		num_fields = mysql_num_fields (mysql_res);
		for (int i = 0; i < num_fields; i++)
		{
			if (lengths[i] != name_len - 1) continue;
			if (strncmp (mysql_row[i], name, RAWTABLENAMELEN)) continue;
			else
			{
				mysql_free_result (mysql_res);
				return 1;
			}
		}
	}
	mysql_free_result(mysql_res);

	return 0;
}

/*create table of day average data with given arg, return 0 if ok, return other if error occur,mysql is 
 * connection having established to mysql server, site is name of place,tyg or tsg
*/
int create_table_day_average (enum SITE sitetype, MYSQL *mysql)
{
	char *enum_string, *audit_enum_string;
	char *query = NULL;
	MYSQL_ROW mysql_row;
	int query_len;
	char *site;
	int ret;
	
	//check arg
	if (mysql == NULL || check_site (sitetype)) return 1; //arg invalid
	if ((site = get_site_name (sitetype)) == NULL) return 6;
	
	//construct the insert query
	if ((enum_string = mysql_enum_type_string()) == NULL) return 2; //return 2 if get enum string fail
	query_len = strlen ("CREATE TABLE IF NOT EXISTS ") + strlen (site) + strlen (DAY_AVERAGE_TABLE_NAME_SUFFIX) + strlen (" (\
day DATE NOT NULL,\
type enum (") + strlen (enum_string) + strlen (") NOT NULL,\
value decimal(28,18) NOT NULL,\
aqi smallint DEFAULT NULL,\
reserved varchar(20) DEFAULT '',\
PRIMARY KEY (day,type)) ENGINE=InnoDB DEFAULT CHARSET=gbk");
	
	if ((query = malloc (query_len)) == NULL)
	{
		free (enum_string);
		return 3; //return 3 if malloc failed
	}
	if ((ret = snprintf (query, query_len, "CREATE TABLE IF NOT EXISTS %s%s (\
day DATE NOT NULL,\
type enum(%s) NOT NULL,\
value decimal(28,18) NOT NULL,\
aqi smallint DEFAULT NULL,\
reserved varchar(20) DEFAULT '',\
PRIMARY KEY (day,type)) ENGINE=InnoDB DEFAULT CHARSET=gbk", 
		site, DAY_AVERAGE_TABLE_NAME_SUFFIX, enum_string)) < 0)
	{
		free (enum_string);
		free (query);
		return 4; //return 4 if sprintf failure
	}
	if ((size_t)ret + 1 > query_len)
	{
		free (enum_string);
		free (query);
		return 4; //return 4 if sprintf failure
	}
	free (enum_string);

	if (mysql_query (mysql, query))
	{
		fprintf (stderr, "create day average table failed,query string : %s\n", query);
		free (query);
		return 5; //return 5 if mysql_query for exist of table failure
	}

	free (query);
	return 0;
}

/*
 * create table of O3 8h moving average max of day,
 * return 0 if ok, return other if error occur
 */
int create_table_O3_8h_moving_average_max (enum SITE sitetype, MYSQL *mysql)
{
	char *query;
	char *site;
	size_t len;
	int ret;
	if (check_site (sitetype) || mysql == NULL) return -1;
	if ((site = get_site_name (sitetype)) == NULL) return -1;
	len = strlen ("CREATE TABLE IF NOT EXISTS ") + strlen (site) + strlen (O3_8H_MOVING_AVERAGE_MAX_TABLE_NAME_SUFFIX) + strlen (" (\
day date NOT NULL,\
value decimal(28,18) NOT NULL,\
aqi smallint DEFAULT NULL,\
reserved varchar(20) DEFAULT '',\
PRIMARY KEY (day)) ENGINE=InnoDB DEFAULT CHARSET=gbk") + 1;
	if ((query = malloc (len)) == NULL) return -2;
	if ((ret = snprintf (query, len, "CREATE TABLE IF NOT EXISTS %s%s (\
day date NOT NULL,\
value decimal(28,18) NOT NULL,\
aqi smallint DEFAULT NULL,\
reserved varchar(20) DEFAULT '',\
PRIMARY KEY (day)) ENGINE=InnoDB DEFAULT CHARSET=gbk", site, O3_8H_MOVING_AVERAGE_MAX_TABLE_NAME_SUFFIX)) < 0)
	{
		free (query);
		return -3;
	}
	if ((size_t)ret + 1 > len)
	{
		free (query);
		return -3;
	}
	if (mysql_real_query (mysql, query, strlen (query)))
	{
		fprintf (stderr, "create table of O3 8h moving average max of day,query:%s\nerrno:%u, error:%s\n", query, mysql_errno (mysql), mysql_error (mysql));
		free (query);
		return -4;
	}
	free (query);
	return 0;
}

/*create table of O3 8h moving average data with given arg, return 0 if ok, return other if error occur,mysql is 
 * connection having established to mysql server, site is name of place,tyg or tsg
*/
int create_table_O3_8h_moving_average (uint16_t year, uint16_t month, enum SITE sitetype, MYSQL *mysql)
{
	char *query = NULL;
	MYSQL_ROW mysql_row;
	char *site;
	size_t query_len;
	int ret;
	
	//check arg
	if (mysql == NULL || year > 9999 || check_site (sitetype)) return 1; //arg invalid
	if ((site = get_site_name (sitetype)) == NULL) return 6;
	
	//construct the insert query
	query_len = strlen ("CREATE TABLE IF NOT EXISTS ") + strlen (site) + strlen ("_") + strlen (O3_8H_MOVING_AVERAGE_TABLE_NAME_PREFIX) + strlen ("YYYYMM") + strlen (" (\
day DATE NOT NULL,\
hour tinyint NOT NULL,\
value decimal(28,18) NOT NULL,\
aqi smallint DEFAULT NULL,\
reserved varchar(20) DEFAULT '',\
PRIMARY KEY (day,hour)) ENGINE=InnoDB DEFAULT CHARSET=gbk") + 1;
	
	if ((query = malloc (query_len)) == NULL)
	{
		return 3; //return 3 if malloc failed
	}
	if ((ret = snprintf (query, query_len, "CREATE TABLE IF NOT EXISTS %s_%s%" PRIu16 "%02" PRIu16 " (\
day DATE NOT NULL,\
hour tinyint NOT NULL,\
value decimal(28,18) NOT NULL,\
aqi smallint DEFAULT NULL,\
reserved varchar(20) DEFAULT '',\
PRIMARY KEY (day,hour)) ENGINE=InnoDB DEFAULT CHARSET=gbk", 
		site, O3_8H_MOVING_AVERAGE_TABLE_NAME_PREFIX, year, month)) < 0)
	{
		free (query);
		return 4; //return 4 if sprintf failure
	}
	if ((size_t)ret + 1 > query_len)
	{
		free (query);
		return 4; //return 4 if sprintf failure
	}

	if (mysql_query (mysql, query))
	{
		fprintf (stderr, "create day average table failed,query string : %s\nerrno:%u,err:%s", query,mysql_errno (mysql), mysql_error (mysql));
		free (query);
		return 5; //return 5 if mysql_query for exist of table failure
	}

	free (query);
	return 0;
}

int create_table_O3_1h_max (enum SITE sitetype, MYSQL *mysql)
{
	char *query;
	size_t len;
	char *site;
	int ret;
	if (check_site (sitetype) || mysql == NULL) return -1; //arg invalid
	if ((site = get_site_name (sitetype)) == NULL) return -1;
	len = strlen ("CREATE TABLE IF NOT EXISTS ") + strlen (site) + strlen (O3_1H_MAX_TABLE_NAME_SUFFIX) + strlen (" (\
day date NOT NULL,\
value decimal(28,18) NOT NULL,\
aqi smallint DEFAULT NULL,\
reserved varchar(20) DEFAULT '',\
PRIMARY KEY (day)) ENGINE=InnoDB DEFAULT CHARSET=gbk") + 1;
	if ((query = malloc (len)) == NULL) return -2;
	if ((ret = snprintf (query, len, "CREATE TABLE IF NOT EXISTS %s%s (\
day date NOT NULL,\
value decimal(28,18) NOT NULL,\
aqi smallint DEFAULT NULL,\
reserved varchar(20) DEFAULT '',\
PRIMARY KEY (day)) ENGINE=InnoDB DEFAULT CHARSET=gbk", site, O3_1H_MAX_TABLE_NAME_SUFFIX)) < 0)
	{
		free (query);
		return -3;
	}
	if ((size_t)ret + 1 > len)
	{
		free (query);
		return -3;
	}
	if (mysql_real_query (mysql, query, strlen (query)))
	{
		free (query);
		return -4;
	}
	free (query);
	return 0;
}

//check whether given table exist, return 1 if exist, 0 not exist, other indicating an error
int is_table_exist (char *table, MYSQL *mysql)
{
	if (table == NULL || mysql == NULL)
	{
		return 2; //return 2 if arg invalid
	}

	//prepare query string
	size_t len = strlen ("SHOW TABLES LIKE '") + strlen (table) + strlen ("'") + 1;
	int ret;
	char *query;
	if ((query = malloc (len)) == NULL) return 3;
	if ((ret = snprintf (query, len, "SHOW TABLES LIKE '%s'", table)) < 0)
	{
		free (query);
		return 3;
	}
	if ((size_t)ret + 1 > len)
	{
		free (query);
		return 3;
	}

	//issue query and save result and retrieve row
	MYSQL_RES *result;
	my_ulonglong nums;

	if (mysql_real_query (mysql, query, len - 1))
	{
		free (query);
		return 4;
	}
	free (query);
	if ((result = mysql_store_result (mysql)) == NULL)
	{
		return 4;
	}
	if ((nums = mysql_num_rows (result)) != (my_ulonglong) 1)
	{
		mysql_free_result (result);
		return 0;
	}
	else
	{
		mysql_free_result (result);
		return 1;
	}
}

/*
 * construct mysql lock query of hour average
 */
char *construct_hour_average_table_lock_mysql_query (uint16_t year, uint16_t month, enum SITE sitetype)
{
	char *query;
	size_t len;
	int ret;
	char *site;
	if (year > 9999 || month > 12 || month < 1 || check_site (sitetype)) return NULL;
	if ((site = get_site_name (sitetype)) == NULL) return NULL;
	len = strlen ("LOCK TABLES ") + SITENAMEMAX + strlen ("_") + strlen (HOUR_AVERAGE_TABLE_NAME_PREFIX) + strlen ("YYYYMM WRITE");
	if ((query = malloc (len)) == NULL) return NULL;
	if ((ret = snprintf (query, len, "LOCK TABLES %s_%s%" PRIu16 "%02" PRIu16 " WRITE", site, HOUR_AVERAGE_TABLE_NAME_PREFIX, year, month)) < 0 || (size_t)ret + 1 > len)
	{
		free (query);
		return NULL;
	}
	return query;
}

/*
 * construct mysql lock query of day average
 */
char *construct_day_average_table_lock_mysql_query (enum SITE sitetype)
{
	char *query;
	size_t len;
	int ret;
	char *site;
	if (check_site (sitetype)) return NULL;
	if ((site = get_site_name (sitetype)) == NULL) return NULL;
	len = strlen ("LOCK TABLES ") + SITENAMEMAX + strlen (DAY_AVERAGE_TABLE_NAME_SUFFIX) + strlen (" WRITE");
	if ((query = malloc (len)) == NULL) return NULL;
	if ((ret = snprintf (query, len, "LOCK TABLES %s%s WRITE", site, DAY_AVERAGE_TABLE_NAME_SUFFIX)) < 0 || (size_t)ret + 1 > len)
	{
		free (query);
		return NULL;
	}
	return query;
}

/*
 * construct mysql lock query of O3 8h
 * moving average
 */
char *construct_O3_moving_average_table_lock_mysql_query (enum SITE sitetype, uint16_t year, uint16_t month)
{
	char *query;
	size_t len;
	int ret;
	char *site;
	if (check_site (sitetype)) return NULL;
	if ((site = get_site_name (sitetype)) == NULL) return NULL;
	len = strlen ("LOCK TABLES ") + SITENAMEMAX + strlen ("_") + strlen (O3_8H_MOVING_AVERAGE_TABLE_NAME_PREFIX) + strlen ("YYYYMM") + strlen (" WRITE") + 1;
	if ((query = malloc (len)) == NULL) return NULL;
	if ((ret = snprintf (query, len, "LOCK TABLES %s_%s%" PRIu16 "%02" PRIu16 " WRITE", site, O3_8H_MOVING_AVERAGE_TABLE_NAME_PREFIX, year, month)) < 0 || (size_t)ret + 1 > len)
	{
		free (query);
		return NULL;
	}
	return query;
}

/*
 * construct mysql lock query of O3 1h max
 */
char *construct_O3_1h_max_table_lock_mysql_query (enum SITE sitetype)
{
	char *query;
	size_t len;
	int ret;
	char *site;
	if (check_site (sitetype)) return NULL;
	if ((site = get_site_name (sitetype)) == NULL) return NULL;
	len = strlen ("LOCK TABLES ") + SITENAMEMAX + strlen (O3_1H_MAX_TABLE_NAME_SUFFIX) + strlen (" WRITE");
	if ((query = malloc (len)) == NULL) return NULL;
	if ((ret = snprintf (query, len, "LOCK TABLES %s%s WRITE", site, O3_1H_MAX_TABLE_NAME_SUFFIX)) < 0 || (size_t)ret + 1 > len)
	{
		free (query);
		return NULL;
	}
	return query;
}

char *construct_O3_moving_average_max_table_lock_mysql_query (enum SITE sitetype)
{
	char *query;
	size_t len;
	int ret;
	char *site;
	if (check_site (sitetype)) return NULL;
	if ((site = get_site_name (sitetype)) == NULL) return NULL;
	len = strlen ("LOCK TABLES ") + SITENAMEMAX + strlen (O3_8H_MOVING_AVERAGE_MAX_TABLE_NAME_SUFFIX) + strlen (" WRITE");
	if ((query = malloc (len)) == NULL) return NULL;
	if ((ret = snprintf (query, len, "LOCK TABLES %s%s WRITE", site, O3_8H_MOVING_AVERAGE_MAX_TABLE_NAME_SUFFIX)) < 0 || (size_t)ret + 1 > len)
	{
		free (query);
		return NULL;
	}
	return query;
}

