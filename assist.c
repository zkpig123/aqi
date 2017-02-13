#include "aqi.h"
#include <time.h>

/*
 * empty string is "" or string only contain
 * space or tab
 */
int is_empty_string (char *s)
{
	if (s[0] == '\0') return 1;
	for (int i = 0; i < strlen (s); i++)
	{
		if (!isblank (i)) return 0;
	}
	return 1;
}

char *get_audit_name (enum AUDIT_TYPE type)
{
	switch (type)
	{
		case B:
			return "B";
			break;
		case BB:
			return "BB";
			break;
		case W:
			return "W";
			break;
		case H:
			return "H";
			break;
		case HSp:
			return "HSp";
			break;
		case LSp:
			return "LSp";
			break;
		case PZ:
			return "PZ";
			break;
		case PS:
			return "PS";
			break;
		case AS:
			return "AS";
			break;
		case CZ:
			return "CZ";
			break;
		case CS:
			return "CS";
			break;
		case Re:
			return "Re";
			break;
		case T:
			return "T";
			break;
		case empty_audit:
			return "";
			break;
		default:
			return NULL;
	}
}

//return corresponding audit type or invalid_audit_type if invalid,
//from_mysql indicate type string is result of mysql query, 0 if not,
//other yes
enum AUDIT_TYPE get_audit_type (char *type, int from_mysql)
{
	if (!from_mysql) if (type == NULL) return empty_audit;
	if (!strncmp (type, "B", sizeof ("B") - 1)) return B;
	if (!strncmp (type, "BB", sizeof ("BB") - 1)) return BB;
	if (!strncmp (type, "W", sizeof ("W") - 1)) return W;
	if (!strncmp (type, "H", sizeof ("H") - 1)) return HSp;
	if (!strncmp (type, "HSp", sizeof ("HSp") - 1)) return HSp;
	if (!strncmp (type, "LSp", sizeof ("LSp") - 1)) return LSp;
	if (!strncmp (type, "PZ", sizeof ("PZ") - 1)) return PZ;
	if (!strncmp (type, "PS", sizeof ("PS") - 1)) return PS;
	if (!strncmp (type, "AS", sizeof ("AS") - 1)) return AS;
	if (!strncmp (type, "CZ", sizeof ("CZ") - 1)) return CZ;
	if (!strncmp (type, "CS", sizeof ("CS") - 1)) return CS;
	if (!strncmp (type, "Re", sizeof ("Re") - 1)) return Re;
	if (!strncmp (type, "T", sizeof ("T") - 1)) return T;
	if (is_empty_string (type)) return empty_audit;
	return invalid_audit_type;
}

//return 0 if valid, other if invalid,
//from_mysql indicate type string is result of mysql query,0 if not,
//other yes
int check_audit_string (char *type, int from_mysql)
{
	if (!from_mysql) if (type == NULL) return 1;
	if (!is_empty_string(type) && strncmp (type, "B", sizeof ("B") - 1) && strncmp (type, "BB", sizeof ("BB") - 1)
	&& strncmp (type, "W", sizeof ("W") - 1) && strncmp (type, "H", sizeof ("H") - 1)
	&& strncmp (type, "HSp", sizeof ("HSp") - 1) && strncmp (type, "LSp", sizeof ("LSp") - 1)
	&& strncmp (type, "PZ", sizeof ("PZ") - 1) && strncmp (type, "PS", sizeof ("PS") - 1)
	&& strncmp (type, "AS", sizeof ("AS") - 1) && strncmp (type, "CZ", sizeof ("CZ") - 1)
	&& strncmp (type, "CS", sizeof ("CS") - 1) && strncmp (type, "RE", sizeof ("RE") - 1)
	&& strncmp (type, "T", sizeof ("T") - 1)) return 1;
	return 0;
}

//return 0 if valid, other if invalid
int check_audit_type (enum AUDIT_TYPE type)
{
	switch (type)
	{
		case B: case BB: case W: case H:  case HSp:
		case LSp: case PZ: case PS: case AS: case CZ:
		case CS: case Re: case T: case empty_audit:
		return 0;

		default:
		return 1;
	}
}

/*
 * whether audit type is type 'Re',meaning data auto retrieved
 * after
 */
int is_retrieved_audit_type (enum AUDIT_TYPE audit)
{
	if (audit == Re) return 1;
	else return 0;
}

/*
 * whether audit type is 'T' meaning like O3 -2 audit to be 2 ug/m3
 */
int is_audited_audit_type (enum AUDIT_TYPE audit)
{
	if (audit == T) return 1;
	else return 0;
}

char *get_audit_chinese (enum AUDIT_TYPE audit)
{
	switch (audit)
	{
		case B:
			return "运行不良";
			break;
		case BB:
			return "连接不良";
			break;
		case W:
			return "等待数据恢复";
			break;
		case H:
			return "有效数据不足";
			break;
		case HSp:
			return "数据超上限";
			break;
		case LSp:
			return "数据超下限";
			break;
		case PZ:
			return "零点检查";
			break;
		case PS:
			return "跨度检查";
			break;
		case AS:
			return "精度检查";
			break;
		case CZ:
			return "零点校准";
			break;
		case CS:
			return "跨度校准";
			break;
		case Re:
			return "仪器回补数据";
			break;
		case T:
			return "修订数据";
			break;
		case empty_audit:
			return "";
			break;
		default:
			return NULL;
	}
}

enum TYPE get_enum_type (char *name)
{
	if (name == NULL) return invalid_type;
	if (!strncmp (name, "SO2", sizeof ("SO2") - 1)) return SO2;
	if (!strncmp (name, "NO2", sizeof ("NO2") - 1)) return NO2;
	if (!strncmp (name, "O3", sizeof ("O3") - 1)) return O3;
	if (!strncmp (name, "CO", sizeof ("CO") - 1)) return CO;
	if (!strncmp (name, "PM10", sizeof ("PM10") - 1))return PM10;
	if (!strncmp (name, "PM25", sizeof ("PM25") - 1)) return PM25;
	if (!strncmp (name, "O3_8h", sizeof ("O3_8h") - 1)) return O3_8h;
	if (!strncmp (name, "O3_8h_max", sizeof ("O3_8h_max") - 1)) return O3_8h_max;
	if (!strncmp (name, "O3_1h_max", sizeof ("O3_1h_max") - 1)) return O3_1h_max;
	if (!strncmp (name, "wind_velocity", sizeof ("wind_velocity") - 1)) return wind_velocity;
	if (!strncmp (name, "ambient_temperature", sizeof ("ambient_temperature") - 1)) return ambient_temperature;
	if (!strncmp (name, "humidity", sizeof ("humidity") - 1)) return humidity;
	if (!strncmp (name, "atom_pressure", sizeof ("atom_pressure") - 1)) return atom_pressure;
	if (!strncmp (name, "wind_direction", sizeof ("wind_direction") - 1)) return wind_direction;
	if (!strncmp (name, "room_temperature", sizeof ("room_temperature") - 1)) return room_temperature;
	if (!strncmp (name, "doas_light_insensity", sizeof ("doas_light_insensity") - 1)) return doas_light_insensity;
	if (!strncmp (name, "doas_monitor_status", sizeof ("doas_monitor_status") - 1)) return doas_monitor_status;
	if (!strncmp (name, "doas_average_count", sizeof ("doas_average_count") - 1)) return doas_average_count;
	if (!strncmp (name, "doas_optical_path", sizeof ("doas_optical_path") - 1)) return doas_optical_path;
	if (!strncmp (name, "doas_so2_sig_noise_ratio", sizeof ("doas_so2_sig_noise_ratio") - 1)) return doas_so2_sig_noise_ratio;
	if (!strncmp (name, "doas_no2", sizeof ("doas_no2") - 1)) return doas_no2_sig_noise_ratio;
	if (!strncmp (name, "doas_o3", sizeof ("doas_o3") - 1)) return doas_o3_sig_noise_ratio;
	if (!strncmp (name, "doas_pda_status", sizeof ("doas_pda_status") - 1)) return doas_pda_status;
	if (!strncmp (name, "doas_elec_machine_status", sizeof ("doas_elec_machine_status") - 1)) return doas_elec_machine_status;
	if (!strncmp (name, "doas_adjust_poll_pos", sizeof ("doas_adjust_poll_pos") - 1)) return doas_adjust_poll_pos;
	if (!strncmp (name, "doas_mercury_lamp_pos", sizeof ("doas_mercury_lamp_pos") - 1)) return doas_mercury_lamp_pos;
	if (!strncmp (name, "doas_mercury_lamp_power", sizeof ("doas_mercury_lamp_power") - 1)) return doas_mercury_lamp_power;
	return invalid_type;

}

/*
 * get upper limit value of given type,return -1 if error,
 * is now only following type has a upper limit in machine,
 * SO2,NO2,O3,CO
*/
double get_type_value_max (enum TYPE type)
{
	switch (type)
	{
		case SO2:
			return SO2_MAX;
			break;
		case NO2:
			return NO2_MAX;
			break;
		case O3:
			return O3_MAX;
			break;
		case CO:
			return CO_MAX;
			break;
		default :
			return -1;
	}
}

/*
 * return max len of type name of all enum type
 */
unsigned int get_type_name_len_max (void)
{
	enum TYPE type;
	unsigned int len = 0;
	for (type = TYPE_ENUM_MIN; type <= TYPE_ENUM_MAX; type++)
	{
		size_t tmp;
		tmp = strlen (get_type_name (type));
		if (len < tmp) len = tmp;
	}
	return len;
}

/*
 * return max len of type name of all enum type
 */
unsigned int get_type_name_len_min (void)
{
	enum TYPE type;
	unsigned int len = get_type_name_len_max();
	for (type = TYPE_ENUM_MIN; type <= TYPE_ENUM_MAX; type++)
	{
		size_t tmp;
		tmp = strlen (get_type_name (type));
		if (len > tmp) len = tmp;
	}
	return len;
}

char* get_type_name (enum TYPE type)
{
	if (type < TYPE_ENUM_MIN || type > TYPE_ENUM_MAX) return NULL;
	switch (type)
	{
		case SO2: return "SO2";
		case NO2: return "NO2";
		case O3: return "O3";
		case CO: return "CO";
		case PM10: return "PM10";
		case PM25: return "PM25";
		case O3_8h: return "O3_8h";
		case O3_8h_max: return "O3_8h_max";
		case O3_1h_max: return "O3_1h_max";
		case wind_velocity: return "wind_velocity";
		case ambient_temperature: return "ambient_temperature";
		case humidity: return "humidity";
		case atom_pressure: return "atom_pressure";
		case wind_direction: return "wind_direction";
		case room_temperature: return "room_temperature";
		case doas_light_insensity: return "doas_light_insensity";
		case doas_monitor_status: return "doas_monitor_status";
		case doas_integral_time: return "doas_integral_time";
		case doas_average_count: return "doas_average_count";
		case doas_optical_path: return "doas_optical_path";
		case doas_so2_sig_noise_ratio: return "doas_so2_sig_noise_ratio";
		case doas_no2_sig_noise_ratio: return "doas_no2_sig_noise_ratio";
		case doas_o3_sig_noise_ratio: return "doas_o3_sig_noise_ratio";
		case doas_sig_noise_ratio: return "doas_sig_noise_ratio"; 
		case doas_pda_status: return "doas_pda_status";
		case doas_elec_machine_status: return "doas_elec_machine_status";
		case doas_adjust_poll_pos: return "doas_adjust_poll_pos";
		case doas_mercury_lamp_pos: return "doas_mercury_lamp_pos";
		case doas_mercury_lamp_power: return "doas_mercury_lamp_power";
		default: return NULL;
	}
}

//check date formatted as yyyymmdd,return 0 if valid, other invalid
int check_date (uint32_t date)
{
	int year,month, day;
	if (date > 99991231) return 1;
	year = date / 10000;
	month = (date % 10000) / 100;
	day = date % 100;
	if (month == 2)
	{
		if (year % 400 == 0 || (year % 4 == 0 && year % 100 != 0))
			if (day > 29) return 1;
		else if (day > 28) return 1;
	}
	switch (month)
	{
		case 1: case 3: case 5: case 7: case 8: case 10: case 12:
		if (day > 31) return 1;
		break;
		default:
		if (day > 30) return 1;
	}
	return 0;
}

//check type, return 0 if valid, other invalid
int check_type_enum (enum TYPE type)
{
	if (type < TYPE_ENUM_MIN || type > TYPE_ENUM_MAX) return 1;
	return 0;
}

//check value in string whether is a valid number
int check_string_of_value (char *value)
{
	int has_dot = 0;
	char *pos;
	pos = value;
	if (pos[0] == '-' || pos[0] == '+') pos++;
	for (int i = 0; i < strlen (pos); i++)
	{
		if (pos[i] == '.')
		{
			if (has_dot == 1) return 1; //not digit
			has_dot = 1;
			continue;
		}
		else if (isdigit (pos[i])) continue;
		else
		{
			return 1; //not digit
		}
	}
	eliminate_fraction_string_trailing_zero (value);
	return 0;
}

/*
 * check time designated "YYYY-MM-DD HH:MM:SS", return 0 if
 * formatted right, or other if invalid
*/
int check_string_of_time_from_mysql (char *time)
{
	for (int i = 0; i < 4; i++)
	{
		if (!isdigit (time[i])) return 1;
	}
	if (time[4] != '-' || time[7] != '-' || !isblank (time[10]) || time[13] != ':' || time[16] != ':') return 1;
	if (!isdigit (time[5]) || !isdigit (time[6]) || !isdigit (time[8]) || !isdigit (time[9]) || !isdigit (time[11]) || !isdigit (time[12]) || !isdigit (time[14]) || !isdigit (time[15]) || !isdigit (time[17]) || !isdigit (time[18])) return 1;
	return 0;
}

/*
 * check string of date formatted "Y(Y)(Y)(Y)-M(M)-D(D)"
 * "Y(Y)(Y)(Y)/M(M)/D(D)",year ranged from 0 to 9999,
 * () means exist or not exist
 */
int check_string_of_date (char *date)
{
	char *pos1, *pos2;
	size_t len = strlen (date);
	char *end = date + len - 1;
	if ((pos1 = strstr(date, "-")) == NULL ||
	(pos2 = strstr (pos1 + 1, "-")) == NULL)
	{
		if ((pos1 = strstr(date, "/")) == NULL ||
		(pos2 = strstr (pos1 + 1, "/")) == NULL)
			return 1;
	}
	if (pos1 - date < 1 || pos1 - date < 2 || pos1 - date > 3 || pos2 - pos1 > 3 || end - pos2 < 1 || end - pos2 > 2) return 1;

	//convert corresponding part to year,month,day and check
	uint16_t year, month, day;
	year = month = day = 0;
	char *ch;
	for (ch = date; ch < pos1; ch++)
	{
		if (!isdigit (*ch)) return 1;
		year *= 10;
		year += (uint16_t) (*ch - '0');
	}
	if (year == 0 || year > 9999) return 1;
	for (ch = pos1 + 1; ch < pos2; ch++)
	{
		if (!isdigit (*ch)) return 1;
		month *= 10;
		month += (uint16_t) (*ch - '0');
	}
	if (month == 0 || month > 12) return 1;
	for (ch = pos2 + 1; ch <= end; ch++)
	{
		if (!isdigit (*ch)) return 1;
		day *= 10;
		day += (uint16_t) (*ch - '0');
	}
	if (day == 0) return 1;
	if (month == 2)
	{
		if (year % 400 == 0 || (year % 4 == 0 && year % 100 != 0))
		{
			if (day > 29) return 1;
		}
		else if (day > 28) return 1;
	}
	else
	{
		switch (month)
		{
			case 4: case 6: case 9: case 11:
			if (day > 30) return 1;
			default:
			if (day > 31) return 1;
		}
	}

	return 0;
}

/*
 * check string of time formatted "H(H):M(M):S(S)"
 * content in '()' can optionally exist, second can be 61 or 62
 * because of existence of leap second
 */
int check_string_of_time (char *time)
{
	char *pos1, *pos2; //get position of ":"
	size_t len = strlen (time);
	char *end = time + len - 1;
	if ((pos1 = strstr(time, ":")) == NULL ||
		(pos2 = strstr (pos1 + 1, ":")) == NULL) return 1;
	if (pos1 - time < 1 || pos1 - time > 2 || pos2 - pos1 < 2 || pos2 - pos1 > 3 || end - pos2 < 1 || end - pos2 > 2) return 1;

	//convert string to hour ,minute ,second and check
	uint16_t hour, minute, second;
	hour = minute = second = 0;
	char *ch;
	for (ch = time; ch < pos1; ch++)
	{
		if (!isdigit (*ch)) return 1;
		hour *= 10;
		hour += (uint16_t) (*ch - '0');
	}
	if (hour > 24) return 1;
	for (ch = pos1 + 1; ch < pos2; ch++)
	{
		if (!isdigit (*ch)) return 1;
		minute *= 10;
		minute += (uint16_t) (*ch - '0');
	}
	if (minute > 59) return 1;
	for (ch = pos2 + 1; ch <= end; ch++)
	{
		if (!isdigit (*ch)) return 1;
		minute *= 10;
		second *= 10;
		second += (uint16_t) (*ch - '0');
	}
	if (second > 62) return 1;
	return 0;

	return 0;
}

//return 0 if valid, other invalid
int check_year_month_day (uint16_t year, uint16_t month, uint16_t day)
{
	if (year >= 9999 || month < 1 || month > 12) return 1; // return 1 if arg invalid
	if (month == 2)
	{
		if (year % 400 == 0 || (year % 4 == 0 && year % 100 != 0))
		{
			if (day > 29) return 1;
		}
		else if (day > 28) return 1;
	}
	else if (month == 4 || month == 6 || month == 9 || month == 11)
	{
		if (day > 30) return 1;
	}
	else if (day > 31) return 1;
	return 0;
}

int check_hour_minute_second (uint16_t hour, uint16_t minute, uint16_t second)
{
	if (hour > 23 || minute > 59 || second > 62) return 1;
	return 0;
}

/*
 * generate a file name corresponding to given arg for log file is current date and time with a
 * format "YYYY-MM-DD-HHMMSS-type.log" , return buffer using malloc or null if failure, caller
 * should free buffer after using this buffer if success, return NULL if failure,
*/
char *log_file_name (enum log_type log)
{
	time_t curtime;
	struct tm* curtm;
	int len;
	char *buf;

	if (log < LOGENUMMIN || log > LOGENUMMAX) return NULL;
	switch (log)
	{
		case import:
		len = sizeof ("YYYY-MM-DD-HHMMSS-" IMPORTLOG ".log");
		break;
		case first_stat:
		len = sizeof ("YYYY-MM-DD-HHMMSS-" FIRSTSTATLOG ".log");
		break;
		case audit:
		len = sizeof ("YYYY-MM-DD-HHMMSS-" AUDITLOG ".log");
		break;
		case second_stat:
		len = sizeof ("YYYY-MM-DD-HHMMSS-" SECONDSTATLOG ".log");
		break;
		default :
		return NULL;
	}

	if ((buf = malloc (len)) == NULL) return NULL;

	curtime = time (NULL);
	if (curtime == (time_t) -1)
	{
		free (buf);
		return NULL;
	}
	curtm = localtime (&curtime);
	if (curtm == NULL)
	{
		free (buf);
		return NULL;
	}
	if (strftime (buf, len, "%Y-%m-%d-%H%M%S", curtm) != sizeof ("YYYY-MM-DD-HHMMSS") - 1)
	{
		free (buf);
		return NULL;
	}
	switch (log)
	{
		case import:
		strncat (buf, "-" IMPORTLOG ".log", sizeof ("-" IMPORTLOG ".log") - 1);
		break;
		case first_stat:
		strncat (buf, "-" FIRSTSTATLOG ".log", sizeof ("-" IMPORTLOG ".log") - 1);
		break;
		case audit:
		strncat (buf, "-" AUDITLOG ".log", len - sizeof ("-" IMPORTLOG ".log") - 1);
		break;
		case second_stat:
		strncat (buf, "-" SECONDSTATLOG ".log", len - sizeof ("-" IMPORTLOG ".log") - 1);
		break;
	}
	buf[len - 1] = '\0';
	return buf;
}

//redirect given stream to file,return 0 if success,other failure
int redirect_stream_to_file (FILE *fp, char *filename)
{
	if (freopen (filename, "w", fp) == NULL) return 1;
	else return 0;
}

/*
 * get last day of today, returned as uint32_t
 * as YYYYMMDD or 0 if arg invalid
*/
uint32_t get_last_day (uint16_t year, uint16_t month, uint16_t day)
{
	if (check_year_month_day (year, month, day)) return 0;
	if (year == 0 && month == 1 && day == 1) return 0;
	if (day == 1)
	{
		switch (month)
		{
			case 1:
			day = 31;
			month = 12;
			year--;
			break;
			case 3:
			if (year % 400 == 0 || (year % 4 == 0 && year % 100 != 0)) day = 29;
			else day = 28;
			month = 2;
			break;
			case 5: case 7: case 10: case 12:
			day = 30;
			month--;
			break;
			case 2: case 4: case 6: case 8: case 9: case 11:
			day = 31;
			month--;
		}
	}
	else day--;
	return (uint32_t) year * 10000 + (uint32_t) month * 100 + (uint32_t)day;
}

/*
 * get tomorrow of given day, returned as uint32_t
 * as YYYYMMDD or 0 if arg invalid
*/
uint32_t get_tomorrow (uint16_t year, uint16_t month, uint16_t day)
{
	if (check_year_month_day (year, month, day)) return 0;
	if (year == 9999 && month == 12 && day == 31) return 0;
	if (day == 31)
	{
		if (month == 12)
		{
			year++;
			month = 1;
			day = 1;
		}
		else
		{
			day = 1;
			month++;
		}
	}
	else if (day == 30)
	{
		if (month == 4 || month == 6 || month == 9 || month == 11)
		{
			month++;
			day = 1;
		}
		else day = 31;
	}
	else if (day == 28 && month == 2)
	{
		if (year % 400 == 0 || (year % 100 != 0 && year % 4 == 0)) day = 29;
		else
		{
			month = 3;
			day = 1;
		}
	}
	else if (day == 29 && month == 2)
	{
		month = 3;
		day = 1;
	}
	else
	{
		day++;
	}
	return (uint32_t) year * 10000 + (uint32_t) month * 100 + (uint32_t)day;
}


//return corresponding hour or 25 indicating given cur_hour invalid
unsigned int get_hour_of_x_hours_ago (uint16_t cur_hour, uint16_t hours)
{
	if (cur_hour == 0 || cur_hour > 24) return 25;
	for (unsigned int i = 0; i < hours; i++)
	{
		if (cur_hour > 1) cur_hour--;
		else cur_hour = 24;
	}
	return (unsigned int) cur_hour;
}

//return string of month formatted "MM"
char * get_month_string (uint16_t month)
{
	switch (month)
	{
		case 1: return "01";break;
		case 2: return "02";break;
		case 3: return "03";break;
		case 4: return "04";break;
		case 5: return "05";break;
		case 6: return "06";break;
		case 7: return "07";break;
		case 8: return "08";break;
		case 9: return "09";break;
		case 10: return "10";break;
		case 11: return "11";break;
		case 12: return "12";break;
		default: return NULL;
	}

}

//return 0 if valid, other invalid
int check_iaqi_type (enum IAQI_TYPE iaqi_type)
{
	if (iaqi_type >= IAQI_TYPE_MIN && iaqi_type <= IAQI_TYPE_MAX)
		return 0;
	else return 1;
}

/*
 * check iaqi type,return 0 if valid ,other invalid
 */
int check_iaqi (uint16_t iaqi)
{
	switch (iaqi)
	{
		case 0: case 50: case 100: case 150: case 200: case 300: case 400: case 500:
		return 0;
		break;
		default:
		return 1;
	}
}

/*
 * check aqi,aqi must ranged 0 to 500,
 * or -1 meaning no value, -2 if 1h average
 * value of so2 greater than 800 then not
 * needed or 8h average value of o3 greater
 * than ? the same,return 0 if valid
 */
int check_aqi (int16_t aqi)
{
	if (aqi <= 500 && aqi >= 0 || aqi == -1 || aqi == -2) return 0;
	else return 1;
}

char *get_site_name (enum SITE site)
{
	if (site == tyg) return "tyg";
	if (site == tsg) return "tsg";
	return NULL;
}

int check_site (enum SITE site)
{
	if (site >= SITE_FIRST && site <= SITE_LAST) return 0;
	else return 1;
}

/*
 * check whether value and audit is matched,
 * return -1 if audit invalid, return 1 if not matched,
 * return 0 if matched
 */
/*int check_value_audit_match (double value, enum AUDIT_TYPE audit)
{
	if (check_audit_type (audit)) return -1;
	switch (audit)
	{
		case B: case BB: case W: case H: case HSp: case LSp: case PZ: case PS: case AS: case CZ: case CS:
		if (value == -1) return 0;
		else return 1;
		break;
		default:
		if (value <= 0) return 1;
		else
		{
			return 0;
		}
	}
}*/

/*
 * check whether value and audit is matched,
 * return -1 if audit invalid, return 1 if not matched,
 * return 0 if matched
 */
int check_value_audit_match (char *value, enum AUDIT_TYPE audit)
{
	if (check_audit_type (audit)) return -1;
	if (check_string_number_of_value (value)) return -1;
	switch (audit)
	{
		case B: case BB: case W: case H: case HSp: case LSp: case PZ: case PS: case AS: case CZ: case CS:
		if (value[0] == '-' && value[1] == '1' && (value[2] == '\0' || (value[2] == '.' && value[3] == '0' && value[4] == '\0'))) return 0; //value is -1 or -1.0
		else return 1;
		break;
		default:
		if (value[0] == '-') return 1; //value less than 0
		else
		{
			return 0;
		}
	}
}

/*
 * round data given in value that was monitoring
 * under normal condition and is less than zero,
 * return 0 if has been modified, return 1 if value
 * is positive not needing modified,return 2
 * indicating invalid cause less than a
 * specific value, return -1 if type invalid
 */
/*int rounding_data (double *value, enum TYPE type)
{
	if (check_type_enum (type)) return -1;
	if (value > 0) return 1;
	switch (type)
	{
		case SO2:
		if (*value > -0.014)
		{
			*value = 0.003;
			return 0;
		}
		break;
		case NO2:
		if (*value > -0.010)
		{
			*value = 0.002;
			return 0;
		}
		break;
		case O3:
		if (*value > -0.01)
		{
			*value = 0.002;
			return 0;
		}
		break;
		case CO:
		if (*value > -1)
		{
			*value = 0.3;
			return 0;
		}
		break;
		case PM10: case PM25:
		if (*value > -0.005)
		{
			*value = 0.002;
			return 0;
		}
		break;
		default:
		return 2;
	}
}*/

enum SITE get_site_enum (char *site)
{
	if (site == NULL) return invalid_site;
	char *sn;
	for (enum SITE i = SITE_FIRST; i <= SITE_LAST; i++)
	{
		size_t len;
		sn = get_site_name (i);
		len = strlen (sn);
		if (strlen(site) != len) return invalid_site;
		if (!strncmp (site, sn, len)) return i;
	}
	return invalid_site;
}

int check_YYYYMMDD_string (char *s)
{
	int v;
	int year, month, day;
	if (strlen (s) != 8) return 1;
	v = atoi (s);
	year = v / 10000;
	month = (v % 10000) / 100;
	day = v % 100;
	if (year < 0 || month <0 || day < 0) return 2;
	if (check_year_month_day ((uint16_t) year, (uint16_t) month, (uint16_t) day)) return 1;
	return 0;
}

/*
 * rounding a floating point,奇进偶不进, position
 * indicate position of rounding at the right direction
 * of the decimal point
 */
int our_rounding (double *value, unsigned int position)
{
	char num[STRING_NUMBER_BUFSIZ];
	unsigned int right = position + 1;
	char *pot_pos;
	if (snprintf (num, STRING_NUMBER_BUFSIZ, "%.12f", *value) < 0) return -1;
	if ((pot_pos = strstr (num, ".")) == NULL) return 0;
	if (pot_pos + position + 1 >= num + STRING_NUMBER_BUFSIZ) return -2; //exceed our ability
	if (position == 0)
	{
		char *d = pot_pos - 1;
		if (pot_pos[1] > '5')
		{
			*pot_pos = '\0';
			*d += 1;
			*value = atoi (num);
			return 0;
		}
		else if (pot_pos[1] < '5')
		{
			*pot_pos = '\0';
			*value = atoi (num);
			return 0;
		}
		else if (atoll (pot_pos + 2) > 0 || (*d - '0') % 2 == 0)
		{
			*pot_pos = '\0';
			*value = atoi (num);
			return 0;
		}
		else
		{
			*pot_pos = '\0';
			*d += 1;
			*value = atoi (num);
			return 0;
		}
	}
	if (pot_pos[right] > '5')
	{
		pot_pos[position]++;
		pot_pos[right] = '\0';
		*value = atof (num);
		return 0;
	}
	else if (pot_pos[right] < 5)
	{
		pot_pos[right] = '\0';
		*value = atof (num);
		return 0;
	}
	else
	{
		if (atoll (pot_pos + right + 1) > 0)
		{
			pot_pos[position]++;
			pot_pos[right] = '\0';
			*value = atof (num);
			return 0;
		}
		else if ((pot_pos[position] - '0') % 2 == 0)
		{
			pot_pos[right] = '\0';
			*value = atof (num);
			return 0;
		}
		else
		{
			pot_pos[position]++;
			pot_pos[right] = '\0';
			*value = atof (num);
			return 0;
		}
	}
}

/*
 * rounding a floating point,奇进偶不进, position
 * indicate position of rounding at the right direction
 * of the decimal point, return -1 if arg invalid, -2 if
 * err occur
 */
int our_string_rounding (char *num, unsigned bytes, unsigned int position)
{
	char add[STRING_NUMBER_BUFSIZ];
	if (num == NULL || bytes < STRING_NUMBER_BUFSIZ) return -1;
	unsigned int right = position + 1;
	char *pot_pos;
	if ((pot_pos = strstr (num, ".")) == NULL) return 0;
	if (pot_pos + position + 1 >= num + bytes) return -2; //exceed our ability
	memset (add, '0', STRING_NUMBER_BUFSIZ);
	if (position == 0)
	{
		char *d = pot_pos - 1;
		long long l;
		if (pot_pos[1] > '5')
		{
			int64_t tmp;
			*pot_pos = '\0';
			tmp = atoll (num);
			tmp++;
			if (tmp / 10000000000 > 0) return -2;
			if (snprintf (num, bytes, "%"PRId64, tmp) < 0) return -3;
			return 0;
		}
		else if (pot_pos[1] < '5')
		{
			*pot_pos = '\0';
			return 0;
		}
		else if ((l = atoll (pot_pos + 2)) > 0 || (*d - '0') % 2 != 0)
		{
			int64_t tmp;
			*pot_pos = '\0';
			tmp = atoll (num);
			tmp++;
			if (tmp / 10000000000 > 0) return -2;
			if (snprintf (num, bytes, "%"PRId64, tmp) < 0) return -3;
			return 0;
		}
		else
		{
			*pot_pos = '\0';
			return 0;
		}
	}
	if (pot_pos[right] > '5')
	{
		pot_pos[right] = '\0';
		if (num[0] == '-')
		{
			add[0] = '-';
			add[1] = '0';
			add[2] = '.';
			add[right + 1] = '1';
			add[right + 2] = '\0';
		}
		else
		{
			add[0] = '0';
			add[1] = '.';
			add[right] = '1';
			add[right + 1] = '\0';
		}
		eliminate_fraction_string_trailing_zero (num);
		if (add_string_number (num, add, num, STRING_NUMBER_BUFSIZ)) return -2;
		eliminate_fraction_string_trailing_zero (num);
		return 0;
	}
	else if (pot_pos[right] < '5')
	{
		pot_pos[right] = '\0';
		eliminate_fraction_string_trailing_zero (num);
		return 0;
	}
	else
	{
		if (atoll (pot_pos + right + 1) > 0)
		{
			pot_pos[right] = '\0';
			if (num[0] == '-')
			{
				add[0] = '-';
				add[1] = '0';
				add[2] = '.';
				add[right + 1] = '1';
				add[right + 2] = '\0';
			}
			else
			{
				add[0] = '0';
				add[1] = '.';
				add[right] = '1';
				add[right + 1] = '\0';
			}
			eliminate_fraction_string_trailing_zero (num);
			if (add_string_number (num, add, num, STRING_NUMBER_BUFSIZ)) return -2;
			eliminate_fraction_string_trailing_zero (num);
			return 0;
		}
		else if ((pot_pos[position] - '0') % 2 == 0)
		{
			pot_pos[right] = '\0';
			eliminate_fraction_string_trailing_zero (num);
			return 0;
		}
		else
		{
			pot_pos[right] = '\0';
			add[0] = '0';
			add[1] = '.';
			add[right] = '1';
			add[right + 1] = '\0';
			for (int i = 2; i < right; i++) add[i] = '0';
			if (add_string_number (num, add, num, STRING_NUMBER_BUFSIZ)) return -2;
			eliminate_fraction_string_trailing_zero (num);
			return 0;
		}
	}
}

/*
 * check number in string, return 0 if valid, return other
 * if invalid,number is from mysql database formatted
 * decimal (18,9), meaning has at most 9 integral part and 9
 * fractional part
 */
int check_string_number_of_value (char *num)
{
	char *dot = NULL;
	char *start = num;
	char *sign = NULL;
	char *first_nonzero = NULL;
	size_t len;
	if (num == NULL || num[0] == '\0') return -1;
	len = strlen (num);
	//if (num[0] == '0' && num[1] == '.' && num[2] == '0' && num[3] == '\0') return -1;
	if (num[0] == '+' || num[0] == '-')
	{
		/*if (num[1] == '0')
		{
			if (num[2] == '\0' || (num[2] == '.' && num[3] == '0' && num[4] == '\0')) return -1;
		}*/
		start++;
		sign = num;
	}
	dot = strstr (num, ".");
	if (dot == num) return -1;
	if (dot != NULL)
	{
		if ((num[0] == '+' || num[0] == '-') && our_atoll (num + 1, dot - num - 1) == 0 && atoll (dot + 1) == 0) return -1;
		for (;start < dot; start++)
		{
			if (!isdigit (*start)) return -2;
			else if (*start == '0')
			{
				if (first_nonzero == NULL && start + 1 != dot) return -7;
			}
			else
			{
				if (first_nonzero == NULL) first_nonzero = start;
			}
		}
		for (start = dot + 1; start < num + len; start++) if (!isdigit (*start))
		{
			return -3;
		}
		if (dot - num > DATA_INTEGRAL_LEN && sign == NULL && first_nonzero == num) return -4;
		else if (dot - num > (DATA_INTEGRAL_LEN + 1) && sign != NULL && first_nonzero == num + 1) return -5;
		if (strlen (dot + 1) > DATA_FRACTION_LEN) return -6;
		return 0;
	}
	else
	{
		if ((num[0] == '+' || num[0] == '-') && atoll (num + 1) == 0) return -1;
		if (len > DATA_INTEGRAL_LEN) return -6;
		for (;start < num + len; start++)
		{
			if (!isdigit (*start)) return -7;
			else if (*start == '0')
			{
				if (*start == '0') if (first_nonzero == NULL && start != num + len - 1) return -8;
			}
			else
			{
				if (first_nonzero == NULL) first_nonzero = start;
			}
		}
		return 0;
	}
}

/*
 * convert number in string pointed by s into longlong,
 * convert at most count characters, if meet invalid
 * character, then convert previously valid number,
 * return 0 if err,return corresponding value if success
 */
long long our_atoll (char *s, unsigned int count)
{
	int sign = 1;
	char *c = s;
	long long int num = 0;
	if (s == NULL || s[0] == '\0') return 0;
	if (s[0] == '-')
	{
		sign = -1;
		c++;
	}
	if (s[0] == '+') c++;
	for (; c <= s + count - 1; c++)
	{
		if (!isdigit (*c)) break;
		num *= 10;
		num += *c - '0';
		if (num > INT64_MAX) return 0; //over flow
	}
	return sign * num;
}

/*
 * add numbers(should be positive) in two string, number is retrieved from
 * mysql database formatted decimal (18.9), so length of
 * number part of two number should less than 19 or 20(negative)
 * ,length of sum should be at least STRING_NUMBER_BUFSIZ,return 0 if ok, return 1 if overflow,
 * return 2 if unknown err
 */
int add_string_number (char *n1, char *n2, char *sum, unsigned int bytes)
{
	char fraction1[DATA_FRACTION_LEN], fraction2[DATA_FRACTION_LEN], fraction[DATA_FRACTION_LEN];
	char *dot1, *dot2;
	uint64_t integer1, integer2, integer;
	char *num1, *num2;
	int ret, i, j;
	int lastzero1, lastzero2, lastzero;
	if (n1 == NULL || n2 == NULL || sum == NULL || bytes < STRING_NUMBER_BUFSIZ || check_string_number_of_value (n1) || check_string_number_of_value (n2))
	{
		return -1;
	}
	num1 = n1;
	num2 = n2;
	if (num1[0] == '-')
	{
		if (num2[0] == '-')
		{
			if (add_string_number (num1 + 1, num2 + 1, sum, bytes)) return -2;
			if (negative_string_number (sum, bytes)) return -3;
			return 0;
		}
		else return substract_string_number (num2, num1 + 1, sum, bytes);
	}
	else if (num2[0] == '-')
	{
		return substract_string_number (num1, num2 + 1, sum, bytes);
	}
	//if (num1[0] == '-' || num2[0] == '-') return -2;
	if (num1[0] == '+') num1++;
	if (num2[0] == '+') num2++;
	if (bytes < STRING_NUMBER_BUFSIZ)
	{
		return -1;
	}
	dot1 = strstr (num1, ".");
	dot2 = strstr (num2, ".");
	if (dot1 == NULL)
	{
		integer1 = atoll (num1);
		fraction1[0] = '\0';
		i = -1;
	}
	else
	{
		integer1 = our_atoll (num1, dot1 - num1);
		char *ch = dot1 + 1;
		for (i = 0, lastzero1 = -1; ch[i] != '\0'; i++)
		{
			if (ch[i] != '0') lastzero1 = -1;
			else
			{
				lastzero1 = i;
			}
			fraction1[i] = ch[i];
		}
		if (lastzero1 != -1)
		{
			fraction1[lastzero1] = '\0';
			i = lastzero1;
		}
		else
		{
			fraction1[i] = '\0';
		}
		i--;
	}
	if (dot2 == NULL)
	{
		integer2 = atoll (num2);
		fraction2[0] = '\0';
		j = -1;
	}
	else
	{
		integer2 = our_atoll (num2, dot2 - num2);
		char *ch = dot2 + 1;
		for (lastzero2 = -1, j = 0; ch[j] != '\0'; j++)
		{
			if (ch[j] != '0') lastzero2 = -1;
			else
			{
				lastzero2 = j;
			}
			fraction2[j] = ch[j];
		}
		if (lastzero2 != -1)
		{
			fraction2[lastzero2] = '\0';
			j = lastzero2;
		}
		else
		{
			fraction2[j] = '\0';
		}
		j--;
	}
	int carry = 0;
	int pos;
	if (i == -1 && j == -1)
	{
		fraction[0] = '\0';
	}
	else
	{
		if (i == -1) pos = j;
		else if (j == -1) pos = i;
		else if (i > j) pos = i;
		else pos = j;
		if (pos < 0) return 2;
		fraction[pos + 1] = '\0';
		int nonzeroafter;
		for (nonzeroafter = 0, lastzero = -1, carry = 0; pos >= 0; pos--)
		{
			int tmp1, tmp2, tmp;
			if (i < pos) tmp1 = '0';
			else tmp1 = fraction1[pos];
			if (j < pos) tmp2 = '0';
			else tmp2 = fraction2[pos];
			tmp = tmp1 - '0' + tmp2 - '0' + carry;
			fraction[pos] = tmp % 10 + '0';
			if (fraction[pos] == '0')
			{
				if (nonzeroafter == 0) lastzero = pos;
			}
			else nonzeroafter = 1;
			carry = 0;
			if (tmp >= 10) carry = 1;
		}
		if (lastzero != -1) fraction[lastzero] = '\0';
	}
	if (carry == 1) integer = integer1 + integer2 + 1;
	else integer = integer1 + integer2;
	if (integer / 10000000000 > 0)
	{
		return 1; //over flow
	}
	if (fraction[0] != '\0')
	{
		if ((ret = snprintf (sum, STRING_NUMBER_BUFSIZ, "%" PRIu64 ".%s", integer, fraction)) < 0 || ret + 1 >  STRING_NUMBER_BUFSIZ)
		{
			return -3;
		}
	}
	else
	{
		if ((ret = snprintf (sum, STRING_NUMBER_BUFSIZ, "%" PRIu64, integer)) < 0 || ret + 1 >  STRING_NUMBER_BUFSIZ)
		{
			return -3;
		}
	}
	return 0;
}

int eliminate_fraction_string_trailing_zero (char *s)
{
	int lastzero = -1, hasnonzero = 0;
	int i;
	char *start, *dot;
	if (s[0] == '-' || s[0] == '+') start = s + 1;
	else start = s;
	dot = NULL;
	for (i = strlen (start) - 1; i >= 0 && start[i] != '.'; i--)
	{
		if (start[i] == '0')
		{
			if (hasnonzero == 0) lastzero = i;
		}
		else hasnonzero = 1;
	}
	if (i >= 0)
	{
		if (lastzero != -1)
		{
			if (lastzero == i + 1) start[i] = '\0';
			else start[lastzero] = '\0';
		}
		dot = strstr (start, ".");
		if (dot == NULL)
		{
			if (s != start)
			{
				if (*start == '0' && start[1] == '\0')
				{
					s[0] = '0';
					s[1] = '\0';
				}
			}
		}
		else
		{
			if (our_atoll (start, dot - start) == 0 && atoll (dot + 1) == 0)
			{
				s[0] = '0';
				s[1] = '\0';
			}
		}
	}
	/*else
	{
		if (s != start)
		{
			if (*start == '0' && *start[1] == '\0')
			{
				s[0] = '0';
				s[1] = '\0';
			}
		}
		//no dot
	}
	dot = strstr (start, ".");
	if (s != start)
	{
		if (dot == NULL)
		{
			if (atoll (start) == 0)
			{
				s[0] = '0';
				s[1] = '\0';
			}
		}
		else
		{
			if (our_atoll (start, dot - start) == 0 && atoll (dot + 1) == 0)
			{
				s[0] = '0';
				s[1] = '\0';
			}
		}
	}*/
}

int divide_string_number_char (char *divisor, char *divide, unsigned int accuracy, char *quotient, unsigned bytes)
{
	char dr[STRING_NUMBER_BUFSIZ];
	char *dot1, *dot2, *ch;
	int bits, divide_fracs, total_bits, i, ret, divisor_frac_bits, frac_bits_dif, sign;
	int64_t div1, div2;
	size_t len1, len2, divisor_int_len;
	if (divisor == NULL || divisor[0] == '\0' || divide == NULL || accuracy < 0 || accuracy > DATA_FRACTION_LEN || quotient == NULL || bytes < STRING_NUMBER_BUFSIZ || check_string_number_of_value (divisor) || check_string_number_of_value (divide)) return -1;
	dot1 = strstr(divisor, ".");
	dot2 = strstr(divide, ".");
	if (dot1 == NULL)
	{
		if (atoll(divisor) == 0)
		{
			quotient[0] = '0';
			quotient[1] = '\0';
			return 0;
		}
	}
	if (dot2 == NULL)
	{
		div2 = atoll (divide);
		if (div2 == 0) return -1;
	}
	if (dot2 == NULL) return divide_string_number (divisor, div2, accuracy, quotient, bytes);
	eliminate_fraction_string_trailing_zero (divide);
	if (dot1 != NULL) eliminate_fraction_string_trailing_zero (divisor);
	len2 = strlen (divide); //len2 is len from start position of divide including sign?//
	divide_fracs = len2 - (dot2 - divide) - 1; //bits of divider after decimal point

	//get bits of divisor before decimal point
	len1 = strlen (divisor); //including sign...
	if (dot1 == NULL) bits = len1; //bits of integral part
	else bits = dot1 - divisor;
	if (divisor[0] == '+' || divisor[0] == '-') bits--;
	total_bits = bits + divide_fracs; //bits of digits including integral part and fractional part
	if (total_bits > DATA_INTEGRAL_LEN) return -2;

	sign = 1;
	ch = divide;
	if (divide[0] == '-')
	{
		sign = -1;
		ch++;
	}
	else if (divide[0] == '+') ch++;
	for (div2 = 0, i = ch - divide; i < len2; ch++, i++)
	{
		if (*ch != '.') div2 = div2 * 10 + *ch - '0';
	}
	if (sign == -1) div2 = -div2;
	if (dot1 == NULL)
	{
		strncpy (dr, divisor, len1);
		for (i = 0, ch = dr + len1; i < divide_fracs; i++) ch[i] = '0';
		ch[i] = '\0';
		return divide_string_number (dr, div2, accuracy, quotient, bytes);
	}
	else
	{
		//get integer of div1 without dot
		int sign = 1;
		ch = divisor;
		if (divisor[0] == '-')
		{
			sign = -1;
			ch++;
		}
		else if (divisor[0] == '+') ch++;
		char *end = divisor + len1 - 1;
		for (div1 = 0; ch <= end; ch++)
		{
			if (*ch != '.') div1 = div1 * 10 + *ch - '0';
		}

		divisor_frac_bits = len1 - (dot1 - divisor) - 1;
		frac_bits_dif = divide_fracs - divisor_frac_bits;


		if (frac_bits_dif >= 0)
		{
			for (i = frac_bits_dif; i >= 1; i--) div1 *= 10;
			if (sign == -1) div1 = -div1;
			if ((ret = snprintf (dr, DATA_INTEGRAL_LEN + 1, "%" PRId64, div1)) < 0 || ret + 1 > DATA_INTEGRAL_LEN) return -3;
			return divide_string_number (dr, div2, accuracy, quotient, bytes);
		}
		else
		{
			int t;
			for (t = div1, bits = 0; t > 0; t /= 10) bits++;
			int left_shift_bits;
			left_shift_bits = -frac_bits_dif;
			if (left_shift_bits >= bits)
			{
				i = 0;
				if (sign == -1) dr[i++] = '-';
				dr[i++] = '0';
				dr[i++] = '.';
				i = left_shift_bits - bits;
				for (int j = left_shift_bits - bits; j > 0; j--)
				{
					dr[i++] = '0';
				}
				if ((ret = snprintf (dr + i, DATA_FRACTION_LEN - i + 1, "%" PRId64, div1)) < 0 || ret > DATA_FRACTION_LEN - i) return -4;
				return divide_string_number (dr, div2, accuracy, quotient, bytes);
			}
			else
			{
				int64_t before_dot, after_dot;
				//get bits of div1
				int tmp;
				for (i = left_shift_bits, tmp = 1; i > 0; i--) tmp *= 10;
				before_dot = div1 / tmp;
				after_dot = div1 % tmp;
				i = 0;
				if (sign == -1) dr[i++] = '-';
				if ((ret = snprintf (dr + i, DATA_INTEGRAL_LEN, "%" PRId64, before_dot)) < 0 || ret + 1 > DATA_INTEGRAL_LEN) return -5;
				i += ret;
				dr[i++] = '.';
				for (tmp /= 10;tmp > 0; after_dot /= 10, tmp /= 10)
				{
					dr[i++] = after_dot / tmp + '0';
				}
				dr[i++] = after_dot + '0';
				dr[i] = '\0';
				//if ((ret = snprintf (dr + i,  DATA_INTEGRAL_LEN, "%" PRId64, after_dot)) < 0 || ret + 1 >  DATA_INTEGRAL_LEN) return -6;
				return divide_string_number (dr, div2, accuracy, quotient, bytes);
			}
		}
	}
}

/*
 * if failed
 */
int divide_string_number (char *divisor, int32_t divide, unsigned int accuracy, char *q, unsigned bytes)
{
	int cur_divisor, cur_quotient, dotpos, no_fraction, len;
	char quotient[STRING_NUMBER_BUFSIZ];
	unsigned int cur_accuracy;
	double ret;
	char *dot;
	char *div, *quot;
	int sign = 1;
	if (divisor == NULL || divisor[0] == '\0' || divide == 0 || accuracy < 0 || accuracy > DATA_FRACTION_LEN || quotient == NULL || bytes < STRING_NUMBER_BUFSIZ || check_string_number_of_value (divisor)) return -1;
	ret = atof (divisor);
	if (ret < 0) ret = -ret;
	if (ret <= DATA_ACCURACY)
	{
		quotient[0] = '0';
		quotient[1] = '\0';
		return 0;
	}
	if (divisor[0] == '+')
	{
		div = divisor + 1;
	}
	else if (divisor[0] == '-')
	{
		div = divisor + 1;
		sign = -1;
	}
	else div = divisor;
	len = (int) strlen (div); //bits of divisor excluding sign
	if (divide < 0)
	{
		sign = -sign;
		divide = -divide;
	}
	/*if (sign == -1)
	{
		quotient[0] = '-';
		quot = quotient + 1;
	}
	else */quot = quotient;
	dot = strstr (div, ".");
	if (dot == NULL) no_fraction = 1;
	else
	{
		dotpos = dot - div; //dotpos relative to divisor,why not div which point to first non sign digit of divisor
		no_fraction = 0;
	}

	cur_accuracy = 0;
	int32_t divdiv = 0;
	cur_divisor = cur_quotient = 0;
	int quotient_has_dot = 0;
	while (1)
	{
		//first part
		for (;divdiv < divide; )
		{
			if (no_fraction == 1)
			{
				if (cur_divisor <= len - 1)
				{
					divdiv = divdiv * 10 + div[cur_divisor] - '0';
					if (divdiv == 0)
					{
						cur_divisor++;
						break;
					}
					else if (divdiv < divide)
					{
						if (cur_divisor == len - 1)
						{
							quot[cur_quotient++] = '0';
							quot[cur_quotient++] = '.';
							quotient_has_dot = 1;
						}
						cur_divisor++;
						continue;
					}
					else
					{
						cur_divisor++;
						break;
					}
				}
				else
				{
					if (cur_divisor == len)
					{
						if (quotient_has_dot == 0)
						{
							quot[cur_quotient++] = '.';
							quotient_has_dot = 1;
						}
					}
					divdiv *= 10;
					cur_divisor++;
					if (divdiv == 0) break;
					else if (divdiv < divide)
					{
						quot[cur_quotient++] = '0';
						cur_accuracy++;
						if (cur_accuracy >= accuracy)
						{
							quot[cur_quotient] = '\0';
							eliminate_fraction_string_trailing_zero (quotient);
							int tmp;
							if (sign == -1)
							{
								if ((tmp = snprintf (q, STRING_NUMBER_BUFSIZ, "-%s", quotient)) < 0 || tmp + 1 > STRING_NUMBER_BUFSIZ) return -2;
							}
							else if ((tmp = snprintf (q, STRING_NUMBER_BUFSIZ, "%s", quotient)) < 0 || tmp + 1 > STRING_NUMBER_BUFSIZ) return -2;
							return 0;
						}
					}
					else break;
				}
			}
			else
			{
				if (cur_divisor < dotpos - 1)
				{
					divdiv = divdiv * 10 + div[cur_divisor] - '0';
					cur_divisor++;
					if (divdiv == 0) break;
					else if (divdiv < divide) continue;
					else break;
				}
				else if (cur_divisor == dotpos)
				{
					quot[cur_quotient++] = '.';
					quotient_has_dot = 1;
					cur_divisor++;
					continue;
				}
				else
				{
					if (cur_divisor < len) divdiv = divdiv * 10 + div[cur_divisor] - '0';
					else divdiv *= 10;
					cur_divisor++;
					if (divdiv == 0) break;
					else if (divdiv < divide)
					{
						quot[cur_quotient++] = '0';
						if (quotient_has_dot == 1) cur_accuracy++;
						if (cur_accuracy >= accuracy)
						{
							quot[cur_quotient] = '\0';
							eliminate_fraction_string_trailing_zero (quotient);
							int tmp;
							if (sign == -1)
							{
								if ((tmp = snprintf (q, STRING_NUMBER_BUFSIZ, "-%s", quotient)) < 0 || tmp + 1 > STRING_NUMBER_BUFSIZ) return -2;
							}
							else
							{
								if ((tmp = snprintf (q, STRING_NUMBER_BUFSIZ, "%s", quotient)) < 0 || tmp + 1 > STRING_NUMBER_BUFSIZ) return -2;
							}
							return 0;
						}
						continue;
					}
					else break;
				}
			}
		}

		//second part
		quot[cur_quotient++] = divdiv / divide + '0';
		if (quotient_has_dot == 1) cur_accuracy++;
		divdiv = divdiv % divide;
		if ((divdiv == 0 && cur_divisor > len - 1) || cur_accuracy >= accuracy) break;
	}
	quot[cur_quotient] = '\0';
	eliminate_fraction_string_trailing_zero (quotient);
	int tmp;
	if (sign == -1)
	{
		if ((tmp = snprintf (q, STRING_NUMBER_BUFSIZ, "-%s", quotient)) < 0 || tmp + 1 > STRING_NUMBER_BUFSIZ) return -2;
	}
	else
	{
		if ((tmp = snprintf (q, STRING_NUMBER_BUFSIZ, "%s", quotient)) < 0 || tmp + 1 > STRING_NUMBER_BUFSIZ) return -2;
	}
	return 0;
}

/*
 * compare two string number, return 0 if equal, return
 * 1 if value1 greater than value2, return -1 if less,
 * return -INT_MAX if arg invalid
 */
int compare_string_number (char *value1, char *value2)
{
	char *v1, *v2, *dot1, *dot2;
	int dotpos1, dotpos2;
	int sign1, sign2;
	sign1 = sign2 = 1;
	if (value1 == NULL || value2 == NULL || check_string_number_of_value (value1) || check_string_number_of_value (value2)) return -INT_MAX;
	if (value1[0] == '+')
	{
		v1 = value1 + 1;
	}
	else if (value1[0] == '-')
	{
		sign1 = -1;
		v1 = value1 + 1;
	}
	else v1 = value1;
	if (value2[0] == '+')
	{
		v2 = value2 + 1;
	}
	else if (value2[0] == '-')
	{
		sign2 = -1;
		v2 = value2 + 1;
	}
	else v2 = value2;
	dot1 = strstr (v1, ".");
	dot2 = strstr (v2, ".");

	//compare digit count between start of number and decimal point
	size_t len1, len2;
	len1 = strlen (v1);
	len2 = strlen (v2);
	if (dot1 != NULL) dotpos1 = dot1 - v1;
	else dotpos1 = len1;
	if (dot2 != NULL) dotpos2 = dot2 - v2;
	else dotpos2 = len2;
	if (dotpos1 > dotpos2)
	{
		if (sign1 == 1) return 1;
		else return -1;
	}
	else if (dotpos1 < dotpos2)
	{
		if (dotpos1 == 1)
		{
			if (dotpos2 == -1)return 1;
			else return -1;
		}
		else
		{
			if (dotpos2 == 1) return -1;
			else return 1;
		}
	}

	//compare each digit number value
	for (int i = 0; i < dotpos1; i++)
	{
		if (v1[i] > v2[i])
		{
			if (sign1 == 1) return 1;
			else return -1;
		}
		else if (v1[i] < v2[i])
		{
			if (sign1 == 1)
			{
				if (sign2 == 1) return -1;
				else return 1;
			}
			else
			{
				if (sign2 == 1) return -1;
				else return 1;
			}
		}
	}
	if (dot1 == NULL)
	{
		if (dot2 == NULL) return 0;
		else
		{
			for (char *ch = dot2 + 1; ch < v2 + len2; ch++)
			{
				if (*ch != '0')
				{
					if (sign2 == 1) return -1;
					else return 1;
				}
			}
			return 0;
		}
	}
	else if (dot2 == NULL)
	{
		for (char *ch = dot1 + 1; ch < v1 + len1; ch++)
		{
			if (*ch != '0')
			{
				if (sign1 == 1) return 1;
				else return -1;
			}
		}
		return 0;
	}
	else
	{
		size_t fraclen1, fraclen2, minlen;
		if (len1 > len2)
		{
			minlen = len2;
		}
		else
		{
			minlen = len1;
		}
		for (int i = dotpos1 + 1; i < minlen; i++)
		{
			if (v1[i] > v2[i])
			{
				if (sign1 == 1) return 1;
				else return -1;
			}
			else if (v1[i] < v2[i])
			{
				if (sign1 == 1)
				{
					if (sign2 == 1) return -1;
					else return 1;
				}
				else
				{
					if (sign2 == 1) return -1;
					else return 1;
				}
			}
		}
		if (len1 == len2) return 0;
		else if (len1 > len2)
		{
			for (int i = len2; i < len1; i++)
			{
				if (v1[i] != '0')
				{
					if (sign1 == 1) return 1;
					else return -1;
				}
			}
			return 0;
		}
		else
		{
			for (int i = len1; i < len2; i++)
			{
				if (v2[i] != '0')
				{
					if (sign2 == 1) return 1;
					else return -1;
				}
			}
			return 0;
		}
	}
	return 0;
}

/*
 * round data given in value that was monitoring
 * under normal condition and is less than zero,
 * return 0 if has been modified, return 1 if value
 * is positive not needing modified,return 2
 * indicating invalid because value is less than a
 * specific value, return -1 if type invalid
 */
int rounding_data (char *value, enum TYPE type)
{
	int ret;
	if (check_type_enum (type) || check_string_number_of_value (value))
	{
		return -1;
	}
	if (( ret = compare_string_number (value, "0")) == 1) return 1;
	switch (type)
	{
		case SO2:
		if (compare_string_number (value, "-0.014") == 1)
		{
			strncpy (value, "0.003", 6);
			return 0;
		}
		break;
		case NO2:
		if (compare_string_number (value, "-0.010") == 1)
		{
			strncpy (value, "0.002", 6);
			return 0;
		}
		break;
		case O3:
		if (compare_string_number (value, "-0.01") == 1)
		{
			strncpy (value, "0.002", 6);
			return 0;
		}
		break;
		case CO:
		if (compare_string_number (value, "-1") == 1)
		{
			strncpy (value, "0.3", 4);
			return 0;
		}
		break;
		case PM10: case PM25:
		if (compare_string_number (value, "-0.005") == 1)
		{
			strncpy (value, "0.002", 6);
			return 0;
		}
		break;
		default:
		return 2;
	}
}

int multiply_string_number (char *value1, char *value2, char *mul, unsigned bytes)
{
	char *dotpos1, *dotpos2;
	size_t len1, len2;
	int digits;
	int sign = 1, ret;
	int64_t v1, v2, v;
	int fraction_bits = 0;
	if (value1 == NULL || value2 == NULL || mul == NULL || check_string_number_of_value (value1) || check_string_number_of_value (value2) || mul == NULL || bytes < STRING_NUMBER_BUFSIZ) return -1;
	dotpos1 = strstr (value1, ".");
	dotpos2 = strstr (value2, ".");
	if (value1[0] == '0' && value1[1] == '\0' || (value2[0] == '0' && value2[1] == '\0'))
	{
		mul[0] = '0';
		mul[1] = '\0';
		return 0;
	}
	/*if (value1[0] = '1' && (value1[1] == '\0' || (value1[1] == '.' && value1[2] == '0' && value1[3] == '\0')))
	{
		strncpy (mul, value2, strlen (value2) + 1);
		return 0;
	}
	if (value2[0] = '1' && (value2[1] == '\0' || (value2[1] == '.' && value2[2] == '0' && value2[3] == '\0')))
	{
		strncpy (mul, value1, strlen (value1) + 1);
		return 0;
	}*/

	if (dotpos1 == NULL) v1 = atoll (value1);
	else
	{
		char *start = value1;
		len1 = strlen (value1);
		if (value1[0] == '+' || (value1[0] == '-' && (sign = -sign) != 0))
		{
			start++;
			if (dotpos1 - value1 > (DATA_INTEGRAL_LEN + 1) || value1 + len1 - dotpos1 > (DATA_FRACTION_LEN / 2 + 1)) return -1;
		}
		else if (dotpos1 - value1 > DATA_INTEGRAL_LEN || value1 + len1 - dotpos1 > (DATA_FRACTION_LEN / 2)) return -1;
		fraction_bits += value1 + len1 - 1 - dotpos1;
		for (v1 = 0; start < value1 + len1; start++)
		{
			if (*start == '.') continue;
			else v1 = v1 * 10 + *start - '0';
		}
	}

	if (dotpos2 == NULL) v2 = atoll (value2);
	else
	{
		char *start = value2;
		len2 = strlen (value2);
		if (value2[0] == '+' || (value2[0] == '-' && (sign = -sign) != 0))
		{
			start++;
			if (dotpos2 - value2 > (DATA_INTEGRAL_LEN + 1) || value2 - len2 - dotpos2 > (DATA_FRACTION_LEN / 2 + 1)) return -1;
		}
		else if (dotpos2 - value2 > DATA_INTEGRAL_LEN || value2 - len2 - dotpos2 > (DATA_FRACTION_LEN / 2)) return -1;
		fraction_bits += value2 + len2 - 1 - dotpos2;
		for (v2 = 0; start < value2 + len2; start++)
		{
			if (*start == '.') continue;
			else v2 = v2 * 10 + *start - '0';
		}
	}
	v = v1 * v2;
	if (sign == -1) v = -v;
	int64_t tmp;
	int cur_mul;
	int dif;
	for (digits = 1, tmp = v; tmp > 0; )
	{
		tmp /= 10;
		if (tmp > 0) digits++;
	}
	dif = fraction_bits - digits;
	cur_mul = 0;
	if (sign == -1) mul[cur_mul++] = '-';
	if (v < 0) v = -v;
	if (dif >= 0)
	{
		mul[cur_mul++] = '0';
		mul[cur_mul++] = '.';
		for (int i = 0; i < dif; i++) mul[cur_mul++] = '0';
		if ((ret = snprintf (mul + cur_mul, bytes - cur_mul, "%" PRId64, v)) < 0 || ret + 1 > bytes) return -2;
		eliminate_fraction_string_trailing_zero (mul);
		return 0;
	}
	else
	{
		dif = -dif;
		int64_t divide;
		for (divide = 1, ret = 0; ret < digits - 1; ret++) divide *= 10;
		for (int i = 0; i < digits + 1 && divide != 0; i++)
		{
			int highest;
			if (i == dif)
			{
				mul[cur_mul++] = '.';
				continue;
			}
			if (divide >= 10)
			{
				mul[cur_mul++] = v / divide + '0';
				v = v % divide;
				divide /= 10;
			}
			else mul[cur_mul++] = v + '0';
		}
		mul[cur_mul] = '\0';
		eliminate_fraction_string_trailing_zero (mul);
		return 0;
	}
}

/*
 * get opposite number, return 0 if ok, return 1 if bytes
 * not enough to hold '-', return -1 if arg invalid
 */
int negative_string_number (char *num, unsigned bytes)
{
	size_t len = strlen (num);
	int i;
	if (num == NULL || len + 1 > bytes || bytes == 0 || check_string_number_of_value (num)) return -1;
	if (num[0] == '0' && num[1] == '\0' || (num[0] == '0' && num[1] == '.' && num[2] == '0' && num[3] == '\0')) return 0;
	if (len + 1 == bytes)
	{
		if (isdigit(num[0])) return 1;
	}
	if (isdigit(num[0]))
	{
		num[len + 1] = '\0';
		for (i = len; i >= 1; i--)
		{
			num[i] = num[i - 1];
		}
		num[0] = '-';
	}
	else if (num[0] == '-')
	{
		for (i = 0; i < len - 1; i++) num[i] = num[i + 1];
		num[len - 1] = '\0';
	}
	else if (num[0] == '+') num[0] = '-';
	return 0;
}

//minuend and substrahend must be positive,return 0 if ok,
int substract_string_number (char *buf1, char *buf2, char *sub, unsigned bytes)
{
	int signsign, sign, ret;
	char v1_frac[DATA_FRACTION_LEN], v2_frac[DATA_FRACTION_LEN], frac[DATA_FRACTION_LEN];
	char v1[STRING_NUMBER_BUFSIZ], v2[STRING_NUMBER_BUFSIZ];
	size_t len1, len2, biglen, maxlen, minlen;
	char *major_frac, *minor_frac;
	char *value1, *value2;
	char *dot1, *dot2;
	int64_t v1_int, v2_int, intdif;
	int64_t *major_int, *minor_int;
	if (buf1 == NULL || buf2 == NULL || check_string_number_of_value (buf1) || check_string_number_of_value (buf2) || bytes < STRING_NUMBER_BUFSIZ || sub == NULL) return -1;
	strcpy (v1, buf1);
	strcpy (v2, buf2);
	eliminate_fraction_string_trailing_zero (v1);
	eliminate_fraction_string_trailing_zero (v2);
	signsign = 1;
	if (v1[0] == '-')
	{
		if (v2[0] == '-')
		{
			if (substract_string_number (v2 + 1, v1 + 1, sub, bytes)) return -2;
			else return 0;
		}
		else
		{
			if (add_string_number (v1 + 1, v2, sub, bytes)) return -3;
			if (negative_string_number (sub, bytes)) return -4;
			return 0;
		}
	}
	else if (v2[0] == '-') return add_string_number (v1, v2 + 1, sub, bytes);
	if (v1[0] == '+') value1 = v1 + 1;
	else value1 = v1;
	if (v2[0] == '+') value2 = v2 + 1;
	else value2 = v2;
	dot1 = strstr (value1, ".");
	dot2 = strstr (value2, ".");
	memset (v1_frac, '0', DATA_FRACTION_LEN);
	memset (v2_frac, '0', DATA_FRACTION_LEN);

	//get integral and fractional part of two values and save integral into integer, fractional into char array
	if (dot1 == NULL)
	{
		v1_int = atoll (value1);
		v1_frac[0] = '\0';
		len1 = 0;
	}
	else
	{
		char *ch;
		int i;
		v1_int = our_atoll (value1, dot1 - value1);
		for (i = 0, ch = dot1 + 1; *ch != '\0'; ch++, i++) v1_frac[i] = *ch;
		len1 = i;
		v1_frac[i] = '\0';
	}
	if (dot2 == NULL)
	{
		v2_int = atoll (value2);
		v2_frac[0] = '\0';
		len2 = 0;
	}
	else
	{
		char *ch;
		int i;
		v2_int = our_atoll (value2, dot2 - value2);
		for (i = 0, ch = dot2 + 1; *ch != '\0'; ch++, i++) v2_frac[i] = *ch;
		v2_frac[i] = '\0';
		len2 = i;
	}

	//compare and set the major to the larger, minor to the minor
	intdif = v1_int - v2_int;
	if (intdif > 0)
	{
		major_frac = v1_frac;
		maxlen = len1;
		minor_frac = v2_frac;
		minlen = len2;
		sign = 1;
	}
	else if (intdif < 0)
	{
		intdif = -intdif;
		major_frac = v2_frac;
		maxlen = len2;
		minor_frac = v1_frac;
		minlen = len1;
		sign = -1;
	}
	else
	{
		if (dot1 == NULL)
		{
			if (dot2 == NULL)
			{
				sub[0] = '0';
				sub[1] = '\0';
				return 0;
			}
			else
			{
				int i;
				char *ch;
				sub[0] = '-';
				sub[1] = '0';
				sub[2] = '.';
				for (i = 3, ch = dot2 + 1; *ch != '\0'; ch++) sub[i++] = *ch;
				sub[i] = '\0';
				eliminate_fraction_string_trailing_zero (sub);
				return 0;
			}
		}
		else
		{
			if (dot2 == NULL)
			{
				int i;
				char *ch;
				sub[0] = '0';
				sub[1] = '.';
				for (i = 2, ch = dot1 + 1; *ch != '\0'; ch++) sub[i++] = *ch;
				sub[i] = '\0';
				eliminate_fraction_string_trailing_zero (sub);
				return 0;
			}
			else
			{
				int64_t t1, t2;
				//compare
				size_t l;
				int comp;
				if (len2 > len1) l = len2;
				else l = len1;
				comp = 0;
				for (int k = 0; k < l; k++)
				{
					if (k > len1 - 1)
					{
						if (our_atoll (v2_frac + k, l - k) == 0)
						{
							comp = 0;
							break;
						}
						else
						{
							comp = -1;
							break;
						}
					}
					if (k > len2 - 1)
					{
						if (our_atoll (v1_frac + k, l - k) == 0)
						{
							comp = 0;
							break;
						}
						else
						{
							comp = 1;
							break;
						}
					}
					if (v1_frac[k] > v2_frac[k])
					{
						comp = 1;
						break;
					}
					else if (v1_frac[k] < v2_frac[k])
					{
						comp = -1;
						break;
					}
				}
				if (comp == 1)
				{
					major_frac = v1_frac;
					maxlen = len1;
					minor_frac = v2_frac;
					minlen = len2;
					sign = 1;
				}
				else if (comp == -1)
				{
					major_frac = v2_frac;
					maxlen = len2;
					minor_frac = v1_frac;
					minlen = len1;
					sign = -1;
				}
				else
				{
					sub[0] = '0';
					sub[1] = '\0';
					return 0;
				}
			}
		}
	}
	if (len1 > len2) biglen = len1;
	else biglen = len2;
	frac[biglen] = '\0';
	int carry = 0;
	int i;
	for (i = biglen - 1; i >= 0; i--)
	{
		int t1, t2;
		int tmp;
		if (i > maxlen - 1) t1 = '0';
		else t1 = major_frac[i];
		if (i > minlen - 1) t2 = '0';
		else t2 = minor_frac[i];
		/*if (major_frac[i] == '\0') t1 = '0';
		else t1 = major_frac[i];*/
		/*if (minor_frac[i] == '\0') t2 = '0';
		else t2 = minor_frac[i];*/
		if ((tmp = t1 + carry - t2) >= 0)
		{
			frac[i] = tmp + '0';
			carry = 0;
		}
		else
		{
			frac[i] = tmp + 10 + '0';
			carry = -1;
		}
	}
	if (carry == -1) intdif--;

	//write to sub
	i = 0;
	if (sign == -1)
	{
		sub[i++] = '-';
	}
	if ((ret = snprintf (sub + i, bytes - i, "%" PRId64, intdif)) < 0 || ret + 1 > bytes - i) return -2;
	i += ret;
	if (atoll (frac) == 0) sub[i++] = '\0';
	else
	{
		sub[i++] = '.';
		strncpy (sub + i, frac, biglen);
		sub[i + biglen] = '\0';
	}
	eliminate_fraction_string_trailing_zero (sub);
	return 0;
}

struct tm* get_time (void)
{
	time_t cur_time;
	struct tm *tm_cur;
	if ((cur_time = time (&cur_time)) == (time_t) -1) return NULL;
	if ((tm_cur = localtime (&cur_time)) == NULL) return NULL;
	return tm_cur;
}
