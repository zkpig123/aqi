#include "aqi.h"

/*
 * get value of IAQI_BOUNDARY,return corresponding value or 
 * -1 if iaqi_type is invalid,-2 if iaqi is invalid, return
 *  DBL_MAX if iaqi_type is 1h average of so2 and iaqi > 200 which 
 *  actually not needed in the standard hj 633-2012, or 
 *  iaqi_type is 8h moving average of so2 and iaqi > 400
 *  ,return -3 if unknown err occur,
 */
char * get_value_of_IAQI_BOUNDARY (enum IAQI_TYPE iaqi_type, int16_t iaqi)
{
	//if (check_iaqi_type (iaqi_type)) return NULL; //invalid iaqi_type
	//if (check_aqi (iaqi)) return NULL; //invalid iaqi
	if (iaqi == 0) return NULL;
	if (iaqi_type == IAQI_SO2_1H)
	{
		switch (iaqi)
		{
			case 50:
			return IAQI_50_SO2_1H;
			case 100:
			return IAQI_100_SO2_1H;
			case 150:
			return IAQI_150_SO2_1H;
			case 200:
			return IAQI_200_SO2_1H;
			case 300: case 400: case 500:
			return NULL;
			default:
			return NULL;
		}
	}
	else if (iaqi_type == IAQI_SO2_24H)
	{
		switch (iaqi)
		{
			case 50:
			return IAQI_50_SO2_24H;
			case 100:
			return IAQI_100_SO2_24H;
			case 150:
			return IAQI_150_SO2_24H;
			case 200:
			return IAQI_200_SO2_24H;
			case 300:
			return IAQI_300_SO2_24H;
			case 400:
			return IAQI_400_SO2_24H;
			case 500:
			return IAQI_500_SO2_24H;
			default:
			return NULL;
		}
	}
	else if (iaqi_type == IAQI_NO2_24H)
	{
		switch (iaqi)
		{
			case 50:
			return IAQI_50_NO2_24H;
			case 100:
			return IAQI_100_NO2_24H;
			case 150:
			return IAQI_150_NO2_24H;
			case 200:
			return IAQI_200_NO2_24H;
			case 300:
			return IAQI_300_NO2_24H;
			case 400:
			return IAQI_400_NO2_24H;
			case 500:
			return IAQI_500_NO2_24H;
			default:
			return NULL;
		}
	}
	else if (iaqi_type == IAQI_NO2_1H)
	{
		switch (iaqi)
		{
			case 50:
			return IAQI_50_NO2_1H;
			case 100:
			return IAQI_100_NO2_1H;
			case 150:
			return IAQI_150_NO2_1H;
			case 200:
			return IAQI_200_NO2_1H;
			case 300:
			return IAQI_300_NO2_1H;
			case 400:
			return IAQI_400_NO2_1H;
			case 500:
			return IAQI_500_NO2_1H;
			default:
			return NULL;
		}
	}
	else if (iaqi_type == IAQI_PM10_24H)
	{
		switch (iaqi)
		{
			case 50:
			return IAQI_50_PM10_24H;
			case 100:
			return IAQI_100_PM10_24H;
			case 150:
			return IAQI_150_PM10_24H;
			case 200:
			return IAQI_200_PM10_24H;
			case 300:
			return IAQI_300_PM10_24H;
			case 400:
			return IAQI_400_PM10_24H;
			case 500:
			return IAQI_500_PM10_24H;
			default:
			return NULL;
		}
	}
	else if (iaqi_type == IAQI_PM25_24H)
	{
		switch (iaqi)
		{
			case 50:
			return IAQI_50_PM25_24H;
			case 100:
			return IAQI_100_PM25_24H;
			case 150:
			return IAQI_150_PM25_24H;
			case 200:
			return IAQI_200_PM25_24H;
			case 300:
			return IAQI_300_PM25_24H;
			case 400:
			return IAQI_400_PM25_24H;
			case 500:
			return IAQI_500_PM25_24H;
			default:
			return NULL;
		}
	}
	else if (iaqi_type == IAQI_CO_24H)
	{
		switch (iaqi)
		{
			case 50:
			return IAQI_50_CO_24H;
			case 100:
			return IAQI_100_CO_24H;
			case 150:
			return IAQI_150_CO_24H;
			case 200:
			return IAQI_200_CO_24H;
			case 300:
			return IAQI_300_CO_24H;
			case 400:
			return IAQI_400_CO_24H;
			case 500:
			return IAQI_500_CO_24H;
			default:
			return NULL;
		}
	}
	else if (iaqi_type == IAQI_CO_1H)
	{
		switch (iaqi)
		{
			case 50:
			return IAQI_50_CO_1H;
			case 100:
			return IAQI_100_CO_1H;
			case 150:
			return IAQI_150_CO_1H;
			case 200:
			return IAQI_200_CO_1H;
			case 300:
			return IAQI_300_CO_1H;
			case 400:
			return IAQI_400_CO_1H;
			case 500:
			return IAQI_500_CO_1H;
			default:
			return NULL;
		}
	}
	else if (iaqi_type == IAQI_O3_1H)
	{
		switch (iaqi)
		{
			case 50:
			return IAQI_50_O3_1H;
			case 100:
			return IAQI_100_O3_1H;
			case 150:
			return IAQI_150_O3_1H;
			case 200:
			return IAQI_200_O3_1H;
			case 300:
			return IAQI_300_O3_1H;
			case 400:
			return IAQI_400_O3_1H;
			case 500:
			return IAQI_500_O3_1H;
			default:
			return NULL;
		}
	}
	else if (iaqi_type == IAQI_O3_8H_MOVING)
	{
		switch (iaqi)
		{
			case 50:
			return IAQI_50_O3_8H;
			case 100:
			return IAQI_100_O3_8H;
			case 150:
			return IAQI_150_O3_8H;
			case 200:
			return IAQI_200_O3_8H;
			case 300:
			return IAQI_300_O3_8H;
			case 400: case 500:
			return NULL;
			default:
			return NULL;
		}
	}
	return NULL;
}
/*double get_value_of_IAQI_BOUNDARY (enum IAQI_TYPE iaqi_type, int16_t iaqi)
{
	if (check_iaqi_type (iaqi_type)) return -1; //invalid iaqi_type
	if (check_aqi (iaqi)) return -2; //invalid iaqi
	if (iaqi == 0) return 0;
	if (iaqi_type == IAQI_SO2_1H)
	{
		switch (iaqi)
		{
			case 50:
			return IAQI_50_SO2_1H;
			case 100:
			return IAQI_100_SO2_1H;
			case 150:
			return IAQI_150_SO2_1H;
			case 200:
			return IAQI_200_SO2_1H;
			case 300: case 400: case 500:
			return DBL_MAX;
			default:
			return -3;
		}
	}
	else if (iaqi_type == IAQI_SO2_24H)
	{
		switch (iaqi)
		{
			case 50:
			return IAQI_50_SO2_24H;
			case 100:
			return IAQI_100_SO2_24H;
			case 150:
			return IAQI_150_SO2_24H;
			case 200:
			return IAQI_200_SO2_24H;
			case 300:
			return IAQI_300_SO2_24H;
			case 400:
			return IAQI_400_SO2_24H;
			case 500:
			return IAQI_500_SO2_24H;
			default:
			return -3;
		}
	}
	else if (iaqi_type == IAQI_NO2_24H)
	{
		switch (iaqi)
		{
			case 50:
			return IAQI_50_NO2_24H;
			case 100:
			return IAQI_100_NO2_24H;
			case 150:
			return IAQI_150_NO2_24H;
			case 200:
			return IAQI_200_NO2_24H;
			case 300:
			return IAQI_300_NO2_24H;
			case 400:
			return IAQI_400_NO2_24H;
			case 500:
			return IAQI_500_NO2_24H;
			default:
			return -3;
		}
	}
	else if (iaqi_type == IAQI_NO2_1H)
	{
		switch (iaqi)
		{
			case 50:
			return IAQI_50_NO2_1H;
			case 100:
			return IAQI_100_NO2_1H;
			case 150:
			return IAQI_150_NO2_1H;
			case 200:
			return IAQI_200_NO2_1H;
			case 300:
			return IAQI_300_NO2_1H;
			case 400:
			return IAQI_400_NO2_1H;
			case 500:
			return IAQI_500_NO2_1H;
			default:
			return -3;
		}
	}
	else if (iaqi_type == IAQI_PM10_24H)
	{
		switch (iaqi)
		{
			case 50:
			return IAQI_50_PM10_24H;
			case 100:
			return IAQI_100_PM10_24H;
			case 150:
			return IAQI_150_PM10_24H;
			case 200:
			return IAQI_200_PM10_24H;
			case 300:
			return IAQI_300_PM10_24H;
			case 400:
			return IAQI_400_PM10_24H;
			case 500:
			return IAQI_500_PM10_24H;
			default:
			return -3;
		}
	}
	else if (iaqi_type == IAQI_PM25_24H)
	{
		switch (iaqi)
		{
			case 50:
			return IAQI_50_PM25_24H;
			case 100:
			return IAQI_100_PM25_24H;
			case 150:
			return IAQI_150_PM25_24H;
			case 200:
			return IAQI_200_PM25_24H;
			case 300:
			return IAQI_300_PM25_24H;
			case 400:
			return IAQI_400_PM25_24H;
			case 500:
			return IAQI_500_PM25_24H;
			default:
			return -3;
		}
	}
	else if (iaqi_type == IAQI_CO_24H)
	{
		switch (iaqi)
		{
			case 50:
			return IAQI_50_CO_24H;
			case 100:
			return IAQI_100_CO_24H;
			case 150:
			return IAQI_150_CO_24H;
			case 200:
			return IAQI_200_CO_24H;
			case 300:
			return IAQI_300_CO_24H;
			case 400:
			return IAQI_400_CO_24H;
			case 500:
			return IAQI_500_CO_24H;
			default:
			return -3;
		}
	}
	else if (iaqi_type == IAQI_CO_1H)
	{
		switch (iaqi)
		{
			case 50:
			return IAQI_50_CO_1H;
			case 100:
			return IAQI_100_CO_1H;
			case 150:
			return IAQI_150_CO_1H;
			case 200:
			return IAQI_200_CO_1H;
			case 300:
			return IAQI_300_CO_1H;
			case 400:
			return IAQI_400_CO_1H;
			case 500:
			return IAQI_500_CO_1H;
			default:
			return -3;
		}
	}
	else if (iaqi_type == IAQI_O3_1H)
	{
		switch (iaqi)
		{
			case 50:
			return IAQI_50_O3_1H;
			case 100:
			return IAQI_100_O3_1H;
			case 150:
			return IAQI_150_O3_1H;
			case 200:
			return IAQI_200_O3_1H;
			case 300:
			return IAQI_300_O3_1H;
			case 400:
			return IAQI_400_O3_1H;
			case 500:
			return IAQI_500_O3_1H;
			default:
			return -3;
		}
	}
	else if (iaqi_type == IAQI_O3_8H_MOVING)
	{
		switch (iaqi)
		{
			case 50:
			return IAQI_50_O3_8H;
			case 100:
			return IAQI_100_O3_8H;
			case 150:
			return IAQI_150_O3_8H;
			case 200:
			return IAQI_200_O3_8H;
			case 300:
			return IAQI_300_O3_8H;
			case 400: case 500:
			return DBL_MAX;
			default:
			return -3;
		}
	}
	return -3;
}*/

/*
 * return iaqi of given iaqi_type and value,return -2 if type is 
 * 1h average of so2 and value greater than value
 * of iaqi boundary 200, or type is 8h moving average of o3 and value 
 * greater than value of iaqi boundary 300, return -1 if value invalid,
 * return -3 if unknown err,return -4 if other err occur
 */
/*
double calc_IAQI (enum IAQI_TYPE iaqi_type, char* value)
{
	double value_low, value_high, iaqi_low, iaqi_high;
	double value_iaqi_50, value_iaqi_100, value_iaqi_150, value_iaqi_200, value_iaqi_300, value_iaqi_400, value_iaqi_500;
	double iaqi;
	if (value <= 0 && value != -1) return -1;
	if (value == -1) return -2;
	if (value < (value_iaqi_50 = get_value_of_IAQI_BOUNDARY (iaqi_type, 50)))
	{
		value_low = IAQI_0_ALL;
		value_high = value_iaqi_50;
		iaqi_low = 0;
		iaqi_high = 50;
	}
	else if (value > value_iaqi_50 && value < (value_iaqi_100 = get_value_of_IAQI_BOUNDARY (iaqi_type, 100)))
	{
		value_low = value_iaqi_50;
		value_high = value_iaqi_100;
		iaqi_low = 50;
		iaqi_high = 100;
	}
	else if (value > value_iaqi_100 && value < (value_iaqi_150 = get_value_of_IAQI_BOUNDARY (iaqi_type, 150)))
	{
		value_low = value_iaqi_100;
		value_high = value_iaqi_150;
		iaqi_low = 100;
		iaqi_high = 150;
	}
	else if (value > value_iaqi_150 && value < (value_iaqi_200 = get_value_of_IAQI_BOUNDARY (iaqi_type, 200)))
	{
		value_low = value_iaqi_150;
		value_high = value_iaqi_200;
		iaqi_low = 150;
		iaqi_high = 200;
	}
	else if (iaqi_type == IAQI_SO2_1H)
	{
		if (value > get_value_of_IAQI_BOUNDARY (IAQI_SO2_1H, 200)) return -2; //return -2 if type is 1h average of so2 and value greater than value of iaqi boundary 200, or type is 8h moving average of o3 and value greater than value of iaqi boundary 300
	}
	else if (iaqi_type == IAQI_O3_8H_MOVING)
	{
		if (value > get_value_of_IAQI_BOUNDARY (IAQI_O3_8H_MOVING, 300)) return -2; 
	}
	else if (value > value_iaqi_200 && value < (value_iaqi_300 = get_value_of_IAQI_BOUNDARY (iaqi_type, 300)))
	{
		value_low = value_iaqi_200;
		value_high = value_iaqi_300;
		iaqi_low = 200;
		iaqi_high = 300;
	}
	else if (value > value_iaqi_300 && value < (value_iaqi_400 = get_value_of_IAQI_BOUNDARY (iaqi_type, 400)))
	{
		value_low = value_iaqi_300;
		value_high = value_iaqi_400;
		iaqi_low = 300;
		iaqi_high = 400;
	}
	else if (value > value_iaqi_400 && value < (value_iaqi_500 = get_value_of_IAQI_BOUNDARY (iaqi_type, 500)))
	{
		value_low = value_iaqi_400;
		value_high = value_iaqi_500;
		iaqi_low = 400;
		iaqi_high = 500;
	}
	else return 500;
	char iaqi_h[STRING_NUMBER_BUFSIZ], iaqi_l[STRING_NUMBER_BUFSIZ], value_h [STRING_NUMBER_BUFSIZ], value_l[STRING_NUMBER_BUFSIZ]; 
	if (snprintf (iaqi_h, STRING_NUMBER_BUFSIZ, "%f", iaqi_high) < 0) return -4;
	if (snprintf (iaqi_l, STRING_NUMBER_BUFSIZ, "%f", iaqi_low) < 0) return -4;
	if (snprintf (value_h, STRING_NUMBER_BUFSIZ, "%f", value_high) < 0) return -4;
	if (snprintf (value_h, STRING_NUMBER_BUFSIZ, "%f", value_high) < 0) return -4;

	iaqi = ((double)iaqi_high - iaqi_low) / (value_high - value_low) * (value - value_low) + iaqi_low;
	if (iaqi > 500) iaqi = 500;
	return iaqi;
}
*/

/*
 * return iaqi of given iaqi_type and value,return -2 if type is 
 * 1h average of so2 and value greater than value
 * of iaqi boundary 200, or type is 8h moving average of o3 and value 
 * greater than value of iaqi boundary 300, return -1 if value invalid,
 * return -3 if unknown err,return -4 if other err occur
 */
int calc_IAQI (enum IAQI_TYPE iaqi_type, char* value)
{
	char *value_low, *value_high;
	int iaqi_low, iaqi_high, ret, compare;
	char *value_iaqi_50, *value_iaqi_100, *value_iaqi_150, *value_iaqi_200, *value_iaqi_300, *value_iaqi_400, *value_iaqi_500;
	int iaqi;
	//if (check_string_number_of_value (value)) return -1;
	//if (value[0] == '0' && (value[1] == '\0' || (value[1] == '.' && value[2] == '0' && value[3] == '\0'))) return -1;
	/*if (value[0] == '-')
	{
		if (value[1] == '1' && (value[2] == '\0'|| (value[2] == '.' && value[3] == '0' && value[4] == '\0'))) return -2;
		else return -1;
	}*/
	if ((compare = compare_string_number (value, value_iaqi_50 = get_value_of_IAQI_BOUNDARY (iaqi_type, 50))) == -1 || compare == 0)
	{
		value_low = IAQI_0_ALL;
		value_high = value_iaqi_50;
		iaqi_low = 0;
		iaqi_high = 50;
	}
	else if ((compare = compare_string_number (value, value_iaqi_100 = get_value_of_IAQI_BOUNDARY (iaqi_type, 100))) == -1 || compare == 0)
	{
		value_low = value_iaqi_50;
		value_high = value_iaqi_100;
		iaqi_low = 50;
		iaqi_high = 100;
	}
	else if ((compare = compare_string_number (value, value_iaqi_150 = get_value_of_IAQI_BOUNDARY (iaqi_type, 150))) == -1 || compare == 0)
	{
		value_low = value_iaqi_100;
		value_high = value_iaqi_150;
		iaqi_low = 100;
		iaqi_high = 150;
	}
	else if ((compare = compare_string_number (value, value_iaqi_200 = get_value_of_IAQI_BOUNDARY (iaqi_type, 200))) == -1 || compare == 0)
	{
		value_low = value_iaqi_150;
		value_high = value_iaqi_200;
		iaqi_low = 150;
		iaqi_high = 200;
	}
	else if (iaqi_type == IAQI_SO2_1H)
	{
		if (compare_string_number (value, get_value_of_IAQI_BOUNDARY (IAQI_SO2_1H, 200)) == 1) return -2; /*return -2 if type is 1h average of so2 and value greater than value of iaqi boundary 200, or type is 8h moving average of o3 and value greater than value of iaqi boundary 300*/
	}
	else if (iaqi_type == IAQI_O3_8H_MOVING)
	{
		if (compare_string_number (value, get_value_of_IAQI_BOUNDARY (IAQI_O3_8H_MOVING, 300)) == 1) return -2; 
	}
	else if ((compare = compare_string_number (value, value_iaqi_300 = get_value_of_IAQI_BOUNDARY (iaqi_type, 300))) == -1 || compare == 0)
	{
		value_low = value_iaqi_200;
		value_high = value_iaqi_300;
		iaqi_low = 200;
		iaqi_high = 300;
	}
	else if ((compare = compare_string_number (value, value_iaqi_400 = get_value_of_IAQI_BOUNDARY (iaqi_type, 400))) == -1 || compare == 0)
	{
		value_low = value_iaqi_300;
		value_high = value_iaqi_400;
		iaqi_low = 300;
		iaqi_high = 400;
	}
	else if ((compare = compare_string_number (value, value_iaqi_500 = get_value_of_IAQI_BOUNDARY (iaqi_type, 500))) == -1 || compare == 0)
	{
		value_low = value_iaqi_400;
		value_high = value_iaqi_500;
		iaqi_low = 400;
		iaqi_high = 500;
	}
	else return 500;
	char value_diff[STRING_NUMBER_BUFSIZ], iaqi_dif[IAQI_LEN + 1], v[STRING_NUMBER_BUFSIZ], tmp[STRING_NUMBER_BUFSIZ], iaqi_low_string[IAQI_LEN + 1];
	if (substract_string_number (value_high, value_low, value_diff, STRING_NUMBER_BUFSIZ))
	{
		return -4;
	}
	//printf ("valuediff:%s\n",value_diff);
	if (substract_string_number (value, value_low, v, STRING_NUMBER_BUFSIZ))
	{
		printf ("sub2\n");
		return -4;
	}
	if ((ret = snprintf (iaqi_dif, IAQI_LEN + 1, "%d", iaqi_high - iaqi_low)) < 0 || ret > IAQI_LEN)
	{
		printf ("snprintf\n");
		return -4;
	}
	//iaqi = ((double)iaqi_high - iaqi_low) / (value_high - value_low) * (value - value_low) + iaqi_low;
	if (divide_string_number_char (iaqi_dif, value_diff, DATA_ACCURACY_BIT, tmp, STRING_NUMBER_BUFSIZ))
	{
		printf ("iaqi_dif:%s!,value_diff:%s!,tmp:%s!\n",iaqi_dif,value_diff,tmp);
		printf ("value_high:%s!,value_low:%s!\n", value_high, value_low);
		printf ("divide\n");
		return -4;
	}
	if (multiply_string_number (tmp, v, tmp, STRING_NUMBER_BUFSIZ))
	{
		printf ("multi\n");
		return -4;
	}
	if ((ret = snprintf (iaqi_low_string, IAQI_LEN + 1, "%d", iaqi_low)) < 0 || ret > IAQI_LEN)
	{
		return -4;
	}
	if ((ret = add_string_number (tmp, iaqi_low_string, tmp, STRING_NUMBER_BUFSIZ)))
	{
		printf ("add\n");
		return -4;
	}
	if (our_string_rounding (tmp, STRING_NUMBER_BUFSIZ, 0))
	{
		printf ("round\n");
		return -4;
	}
	if (sscanf (tmp, "%d", &iaqi) != 1)
	{
		printf ("sscanf err: tmp:%s\n", tmp);
		return -4;
	}
	else
	{
		if (iaqi > 500) iaqi = 500;
		return iaqi;
	}
}


char * construct_O3_8h_moving_average_insert_string (uint16_t year, uint16_t month, uint16_t day, uint16_t hour, char *value, char *reserved_string, enum SITE sitetype, int replace)
{
	char *query;
	size_t len;
	char *reserved = "";
	int ret;
	char *site;
	int16_t aqi;
	/*if (check_year_month_day (year, month, day) || hour > 24 || hour == 0 || value == NULL || check_site (sitetype)) return NULL;
	if (reserved_string != NULL) reserved == reserved_string;
	if (value[0] == '-' && strncmp (value, "-1", 3) && strncmp (value, "-1.0", 3)) return NULL;*/
	char O3_max_string[STRING_NUMBER_BUFSIZ];
	if ((ret = snprintf (O3_max_string, STRING_NUMBER_BUFSIZ, "%d", O3_MAX)) < 0 || ret + 1 > STRING_NUMBER_BUFSIZ) return NULL;
	if (compare_string_number (value, O3_max_string) == 1) return NULL;
	if ((site = get_site_name (sitetype)) == NULL) return NULL;
	aqi = calc_IAQI (IAQI_O3_8H_MOVING, value);
	if (aqi == -1 || aqi == -3 || aqi == -4) return NULL;

	if (!replace) len = strlen ("INSERT IGNORE INTO ") + strlen (site) + strlen ("_") + strlen (O3_8H_MOVING_AVERAGE_TABLE_NAME_PREFIX) + strlen ("YYYYMM") + strlen (" (day,hour,value,aqi,reserved) VALUES ('YYYY-MM-DD',HH,1.XXXXXXXXXXXXXXXXXXXXXXXXXXXX+eXXXXX,XXX,'XXXXXXXXXXXXXXXXXXXX')") + 1;
	else len = strlen ("REPLACE INTO ") + strlen (site) + strlen ("_") + strlen (O3_8H_MOVING_AVERAGE_TABLE_NAME_PREFIX) + strlen ("YYYYMM") + strlen (" (day,hour,value,aqi,reserved) VALUES ('YYYY-MM-DD',HH,1.XXXXXXXXXXXXXXXXXXXXXXXXXXXX+eXXXXX,XXX,'XXXXXXXXXXXXXXXXXXXX')") + 1;
	if ((query = malloc (len)) == NULL) return NULL;
	if (!replace)
	{
		if (aqi != -2) ret = snprintf (query, len, "INSERT IGNORE INTO %s_%s%" PRIu16 "%02" PRIu16 " (day,hour,value,aqi,reserved) VALUES ('%" PRIu16 "-%02" PRIu16 "-%02" PRIu16 "',%02" PRIu16 ",%s,%d,'%s')", site, O3_8H_MOVING_AVERAGE_TABLE_NAME_PREFIX, year, month, year, month, day, hour, value, (int)aqi, reserved);
		else ret = snprintf (query, len, "INSERT IGNORE INTO %s_%s%" PRIu16 "%02" PRIu16 " (day,hour,value,reserved) VALUES ('%" PRIu16 "-%02" PRIu16 "-%02" PRIu16 "',%02" PRIu16 ",%s,'%s')", site, O3_8H_MOVING_AVERAGE_TABLE_NAME_PREFIX, year, month, year, month, day, hour, value, reserved);
		if (ret < 0)
		{
			free (query);
			return NULL;
		}
		if ((size_t)ret + 1 > len)
		{
			free (query);
			return NULL;
		}
	}
	else
	{
		if (aqi == -2) ret = snprintf (query, len, "REPLACE INTO %s_%s%" PRIu16 "%02" PRIu16 " (day,hour,value,reserved) VALUES ('%" PRIu16 "-%02" PRIu16 "-%02" PRIu16 "',%02" PRIu16 ",%s,'%s')", site, O3_8H_MOVING_AVERAGE_TABLE_NAME_PREFIX, year, month, year, month, day, hour, value, reserved);
		else ret = snprintf (query, len, "REPLACE INTO %s_%s%" PRIu16 "%02" PRIu16 " (day,hour,value,aqi,reserved) VALUES ('%" PRIu16 "-%02" PRIu16 "-%02" PRIu16 "',%02" PRIu16 ",%s,%d,'%s')", site, O3_8H_MOVING_AVERAGE_TABLE_NAME_PREFIX, year, month, year, month, day, hour, value, (int) aqi, reserved);
		if (ret < 0);
		{
			free (query);
			return NULL;
		}
		if ((size_t)ret + 1 > len)
		{
			free (query);
			return NULL;
		}
	}
	return query;
}


/*
 * normally table_name is "O3_8h_moving_average", but given in arg table_name
 * construct query string for query 8h moving average data of O3, return buffer
 * containing the query string or NULL if error occur
 * O3_8h_moving_average table naming : 
 * O3_8h_moving_average (day date, hour tinyint, type enum(..), value decimal(18,9), initial decimal(18,9), audit_type enum(...), reserved varchar(20)
*/
char * construct_O3_8h_moving_average_max_from_moving_average_mysql_query (uint16_t year, uint16_t month, uint16_t day, enum SITE sitetype)
{
	char *query; //pointer to buffer
	size_t len; //length of query string
	int ret;
	char *site;

	//check arg
	/*if (check_year_month_day (year, month, day) || check_site (sitetype)) return NULL;
	if ((site = get_site_name (sitetype)) == NULL) return NULL;*/
	
	//calc query string len
	len = strlen ("SELECT value FROM ") + strlen (site) + strlen ("_") + strlen (O3_8H_MOVING_AVERAGE_TABLE_NAME_PREFIX) + strlen ("YYYYMM") + strlen (" WHERE day='YYYY-MM-DD' AND hour BETWEEN 8 AND 24") + 1;
	
	//malloc buffer
	if (len <= 0 || (query = malloc (len)) == NULL) return NULL;
	if ((ret = snprintf (query, len, "SELECT value FROM %s_%s%" PRIu16 "%02" PRIu16 " WHERE day='%" PRIu16 "-%02" PRIu16 "-%02" PRIu16 "' AND hour BETWEEN 8 AND 24", site, O3_8H_MOVING_AVERAGE_TABLE_NAME_PREFIX, year, month, year, month, day)) < 0)
	{
		free (query);
		return NULL;
	}
	if ((size_t)ret + 1 > len)
	{
		free (query);
		return NULL;
	}
	
	//return buffer
	return query;
}


/*
 * construct O3_8h_moving_average max insert string, replace indicate 
 * whether replace if already exist,
 */
char * construct_O3_8h_moving_average_max_insert_string (uint16_t year, uint16_t month, uint16_t day, char *value, char *reserved_string, enum SITE sitetype, int replace)
{
	char *query;
	size_t len;
	char *reserved = "";
	int ret;
	char *site;
	int16_t aqi;

	/*if (check_year_month_day (year, month, day) || value == NULL || check_site (sitetype)) return NULL;
	if (reserved_string != NULL) reserved == reserved_string;
	if (value[0] == '-' && strncmp (value, "-1", 3) && strncmp (value, "-1.0", 5)) return NULL;*/
	if ((site = get_site_name (sitetype)) == NULL) return NULL;
	aqi = calc_IAQI (IAQI_O3_8H_MOVING, value);
	if (aqi == -1 || aqi == -3 || aqi == -4 || !strncmp (value, "-3", 3) || !strncmp (value, "-3.0", 5)) return NULL;

	if (!replace) len = strlen ("INSERT IGNORE INTO ") + strlen (site) + strlen (O3_8H_MOVING_AVERAGE_MAX_TABLE_NAME_SUFFIX) + strlen (" (day,value,aqi,reserved) VALUES ('YYYY-MM-DD',1.XXXXXXXXXXXXXXXXXXXXXXXXXXXX+eXXXXX,XXX,'XXXXXXXXXXXXXXXXXXXX')") + 1;
	else len = strlen ("REPLACE INTO ") + strlen (site) + strlen (O3_8H_MOVING_AVERAGE_MAX_TABLE_NAME_SUFFIX) + strlen (" (day,value,aqi,reserved) VALUES ('YYYY-MM-DD',1.XXXXXXXXXXXXXXXXXXXXXXXXXXXX+eXXXXX,XXX,'XXXXXXXXXXXXXXXXXXXX')") + 1;
	if ((query = malloc (len)) == NULL) return NULL;
	if (!replace)
	{
		if (aqi != -2)
		{
			ret = snprintf (query, len, "INSERT IGNORE INTO %s%s (day,value,aqi,reserved) VALUES ('%" PRIu16 "-%02" PRIu16 "-%02" PRIu16 "',%s,%d,'%s')", site, O3_8H_MOVING_AVERAGE_MAX_TABLE_NAME_SUFFIX, year, month, day, value, (int) aqi, reserved);
		}
		else ret = snprintf (query, len, "INSERT IGNORE INTO %s%s (day,value,reserved) VALUES ('%" PRIu16 "-%02" PRIu16 "-%02" PRIu16 "',%s,%d,'%s')", site, O3_8H_MOVING_AVERAGE_MAX_TABLE_NAME_SUFFIX, year, month, day, value, reserved);
		if (ret < 0)
		{
			free (query);
			return NULL;
		}
		if ((size_t)ret + 1 > len)
		{
			free (query);
			return NULL;
		}
	}
	else
	{
		if (aqi != -2)
		{
			ret = snprintf (query, len, "REPLACE INTO %s%s (day,value,aqi, reserved) VALUES ('%" PRIu16 "-%02" PRIu16 "-%02" PRIu16 "',%s,%d,'%s')", site, O3_8H_MOVING_AVERAGE_MAX_TABLE_NAME_SUFFIX, year, month, day, value, (int) aqi, reserved);
		}
		else ret = snprintf (query, len, "REPLACE INTO %s%s (day,value,reserved) VALUES ('%" PRIu16 "-%02" PRIu16 "-%02" PRIu16 "',%s,%d,'%s')", site, O3_8H_MOVING_AVERAGE_MAX_TABLE_NAME_SUFFIX, year, month, day, value, reserved);
		if (ret < 0)
		{
			free (query);
			return NULL;
		}
		if ((size_t)ret + 1 > len)
		{
			free (query);
			return NULL;
		}
	}
	return query;
}


/*
 * construct_O3_8h_moving_average_calc query before hour 8 meaning 1-7
*/
char * construct_O3_8h_moving_average_8_hours_mysql_query (uint16_t year, uint16_t month, uint16_t day, uint16_t hour, enum SITE sitetype)
{
	char *query; //pointer to buffer
	size_t len; //length of query string
	char *site;
	int ret;

	//check arg
	//if (check_year_month_day (year, month, day) || hour > 24 || hour == 0 || check_site (sitetype)) return NULL;
	if ((site = get_site_name (sitetype)) == NULL) return NULL;

	if (hour >= 8 && hour <= 24)
	{
		len = strlen ("SELECT value,audit_type, FROM ") + strlen (site) + strlen ("_") + strlen (HOUR_AVERAGE_TABLE_NAME_PREFIX) + strlen ("YYYYMM") + strlen (" WHERE day='YYYY-MM-DD' AND type='O3' AND hour between XX and XX") + 1;
		if ((query = malloc (len)) == NULL) return NULL;
		if ((ret = snprintf (query, len, "SELECT value,audit_type FROM %s_%s%" PRIu16 "%02" PRIu16 " WHERE day='%" PRIu16 "-%02" PRIu16 "-%02" PRIu16 "' AND type='O3' AND hour between %02" PRIu16 " and %02" PRIu16, site, HOUR_AVERAGE_TABLE_NAME_PREFIX, year, month, year, month, day, hour - 7, hour)) < 0)
		{
			free (query);
			return NULL;
		}
		if (ret <= len) query[ret] = '\0';
		else
		{
			free (query);
			return NULL;
		}
		return query;
	}

	else
	{
		uint16_t hour_min_lastday, lastyear, lastmonth, lastday;
		uint32_t last;
		if ((hour_min_lastday = get_hour_of_x_hours_ago (hour, 7)) == 25) return NULL;
		if (day == 1)
		{
			if ((last = get_last_day (year, month, day)) == 0) return NULL;
			lastyear = (uint16_t) (last / 10000);
			lastmonth = (uint16_t) ((last % 10000) / 100);
			lastday = (uint16_t) (last % 100);
		}
		else
		{
			lastyear = year;
			lastmonth = month;
			lastday = day - 1;
		}
		
		//calc query string len
		len = strlen ("SELECT value,audit_type FROM ") + strlen (site) + strlen ("_") + strlen (HOUR_AVERAGE_TABLE_NAME_PREFIX) + strlen ("YYYYMM") + strlen (" WHERE day='YYYY-MM-DD' AND type='O3' AND hour between HH and 24 union (select value,audit_type FROM ") + strlen (site) + strlen ("_") + strlen (HOUR_AVERAGE_TABLE_NAME_PREFIX) + strlen ("YYYYMM") + strlen (" WHERE day='YYYY-MM-DD' and type='O3' and hour between 1 and HH)") + 1;
		if ((query = malloc (len)) == NULL) return NULL;
		if ((ret = snprintf (query, len, "SELECT value,audit_type FROM %s_%s%" PRIu16 "%02" PRIu16 " WHERE day='%" PRIu16 "-%02" PRIu16 "-%02" PRIu16 "' AND type='O3' AND hour between %02" PRIu16 " and 24 UNION (SELECT value,audit_type FROM %s_%s%" PRIu16 "%02" PRIu16 " WHERE day='%" PRIu16 "-%02" PRIu16 "-%02" PRIu16 "' AND type='O3' AND hour BETWEEN 1 and %02" PRIu16 ")", site, HOUR_AVERAGE_TABLE_NAME_PREFIX, lastyear, lastmonth, lastyear, lastmonth, lastday, hour_min_lastday, site, HOUR_AVERAGE_TABLE_NAME_PREFIX, year, month, year, month, day, hour)) < 0)
		{
			free (query);
			return NULL;
		}
		if (ret >= len)
		{
			free (query);
			return NULL;
		}
		else
		{
			query[ret] = '\0';
			return query;
		}
	}

	//return buffer
	return query;
}


/*
 * replace indicate whether replace if exist one
 */
char * construct_insert_day_average_mysql_query (uint16_t year, uint16_t month, uint16_t day, enum TYPE type, char *value, char *reserved_string, enum SITE sitetype, int replace)
{
	size_t query_len = 0;
	char *type_string;
	char *query;
	char *reserved = "";
	int ret;
	char *site;
	int16_t aqi;

	/*if (check_year_month_day (year, month, day) || check_site (sitetype) || value == NULL || check_string_number_of_value (value))
	{
		return NULL;
	}
	if (value[0] == '-' && strncmp (value, "-1", 3) && strncmp (value, "-1.0", 5))
	{
		return NULL;
	}*/
	if ((type_string = get_type_name(type)) == NULL)
	{
		return NULL;
	}
	if (reserved_string != NULL)
	{
		reserved = reserved_string;
		if (strlen (reserved) > 20) return NULL;
	}
	if ((site = get_site_name (sitetype)) == NULL)
	{
		return NULL;
	}
	switch (type)
	{
		case SO2:
		aqi = calc_IAQI (IAQI_SO2_24H, value);
		break;
		case NO2:
		aqi = calc_IAQI (IAQI_NO2_24H, value);
		break;
		case CO:
		aqi = calc_IAQI (IAQI_CO_24H, value);
		break;
		case PM10:
		aqi = calc_IAQI (IAQI_PM10_24H, value);
		break;
		case PM25:
		aqi = calc_IAQI (IAQI_PM25_24H, value);
		break;
		default:
		return NULL;
	}
	if (aqi == -1 || aqi == -3 || aqi == -4)
	{
		return NULL;
	}

	if (replace == 0) query_len = strlen ("INSERT IGNORE INTO ") + strlen (site) + strlen (DAY_AVERAGE_TABLE_NAME_SUFFIX) + strlen (" (day,type,value,aqi,reserved) VALUES ('YYYY-MM-DD','") + strlen (type_string) + strlen ("',1.XXXXXXXXXXXXXXXXXXXXXXXXXXXX+eXXXXX,XXX,'XXXXXXXXXXXXXXXXXXXX')") + 1;
	else query_len = strlen ("REPLACE INTO ") + strlen (site) + strlen (DAY_AVERAGE_TABLE_NAME_SUFFIX) + strlen (" (day,type,value,aqi,reserved) VALUES ('YYYY-MM-DD','") + strlen (type_string) + strlen ("',1.XXXXXXXXXXXXXXXXXXXXXXXXXXXX+eXXXXX,XXX,'XXXXXXXXXXXXXXXXXXXX')") + 1;
	if ((query = malloc (query_len)) == NULL)
	{
		return NULL;
	}
	if (replace == 0)
	{
		if (aqi == -2) ret = snprintf (query, query_len, "INSERT IGNORE INTO %s%s (day,type,value,reserved) VALUES ('%" PRIu16 "-%02" PRIu16 "-%02" PRIu16 "','%s',%s,'%s')",
			site, DAY_AVERAGE_TABLE_NAME_SUFFIX, year, month, day, type_string, value, reserved);
		else ret = snprintf (query, query_len, "INSERT IGNORE INTO %s%s (day,type,value,aqi,reserved) VALUES ('%" PRIu16 "-%02" PRIu16 "-%02" PRIu16 "','%s',%s,%d,'%s')",
			site, DAY_AVERAGE_TABLE_NAME_SUFFIX, year, month, day, type_string, value, (int)aqi, reserved);
		if (ret < 0)
		{
			free (query);
			return NULL;
		}
		if ((size_t)(ret + 1) > query_len)
		{
			free (query);
			return NULL;
		}
	}
	else
	{
		if (aqi == -2) ret = snprintf (query, query_len, "REPLACE INTO %s%s (day,type,value,reserved) VALUES ('%4" PRIu16 "-%02" PRIu16 "-%02" PRIu16 "','%s',%s,'%s')",
			site, DAY_AVERAGE_TABLE_NAME_SUFFIX, year, month, day, type_string, value, reserved);
		else ret = snprintf (query, query_len, "REPLACE INTO %s%s (day,type,value,aqi,reserved) VALUES ('%4" PRIu16 "-%02" PRIu16 "-%02" PRIu16 "','%s',%s,%d,'%s')",
			site, DAY_AVERAGE_TABLE_NAME_SUFFIX, year, month, day, type_string, value, (int)aqi, reserved);
		if (ret < 0)
		{
			free (query);
			return NULL;
		}
		if ((size_t)(ret + 1) > query_len)
		{
			free (query);
			return NULL;
		}
	}
	return query;
}


/*construct query string for insert hour average into database,
 * return pointer to that string, caller should free it after
 *  using it, return NULL if error occur or failure, note hour is between
 *  1 and 24,replace is 0 indicating that if exist value not replace
*/
char * construct_hour_average_insert_mysql_query (uint16_t year, uint16_t month, uint16_t day, uint16_t hour, enum TYPE type, char *value, enum AUDIT_TYPE audit_type, char *reserved_string, enum SITE sitetype, int replace)
{
	size_t query_len = 0;
	char *type_string, *audit_type_string;
	char *query;
	char *site;
	char *reserved = "";
	int16_t aqi;
	int ret;

	/*if (check_year_month_day (year, month, day) || hour > 24 || hour == 0 || check_site (sitetype) || check_type_enum (type) || check_audit_type (audit_type) || check_string_number_of_value (value) || check_value_audit_match (value, audit_type))
	{
		return NULL;
	}*/
	if ((site = get_site_name(sitetype)) == NULL)
	{
		return NULL;
	}
	if (reserved_string != NULL && reserved_string[0] != '\0')
	{
		reserved = reserved_string;
		if (strlen (reserved) > 20) return NULL;
	}
	if ((type_string = get_type_name(type)) == NULL) return NULL;
	if((audit_type_string = get_audit_name (audit_type)) == NULL)
	{
		return NULL;
	}
	switch (type)
	{
		case SO2:
		if ((aqi = calc_IAQI (IAQI_SO2_1H, value)) == -1 || aqi == -3 || aqi == -4)
		{
			printf ("so2 aqi:%d\n", aqi);
			return NULL;
		}
		if ((ret = compare_string_number (value, "0.8")) == 1) aqi = -2;
		else if (ret == -INT_MAX) return NULL;
		break;
		case NO2:
		if ((aqi = calc_IAQI (IAQI_NO2_1H, value)) == -1 || aqi == -3 || aqi == -4)
		{
			printf ("no2 aqi:%d\n", aqi);
			return NULL;
		}
		break;
		case O3:
		if ((aqi = calc_IAQI (IAQI_O3_1H, value)) == -1 || aqi == -3 || aqi == -4)
		{
			printf ("o3 aqi:%d\n", aqi);
			return NULL;
		}
		break;
		case CO:
		if ((aqi = calc_IAQI (IAQI_CO_1H, value)) == -1 || aqi == -3 || aqi == -4)
		{
			printf ("co aqi:%d\n", aqi);
			return NULL;
		}
		break;
		case PM10: case PM25:
		aqi = -2;
	}

	if (replace == 0) query_len = strlen ("INSERT IGNORE INTO ") + strlen (site) + strlen ("_") + strlen (HOUR_AVERAGE_TABLE_NAME_PREFIX) + strlen ("YYYYMM (day,hour,type,value,audit_type,aqi,reserved) VALUES ('YYYY-MM-DD',HH,'") + strlen (type_string) + strlen ("',1.XXXXXXXXXXXXXXXXXXXXXXXXXXXXe+xxxxx'") + strlen (audit_type_string) + strlen ("',4AQI,'") + RESERVED_LEN + strlen ("')") + 1; //get length of query string
	else query_len = strlen ("REPLACE INTO ") + strlen (site) + strlen ("_") + strlen (HOUR_AVERAGE_TABLE_NAME_PREFIX) + strlen ("YYYYMM (day,hour,type,value,audit_type,aqi,reserved) VALUES ('YYYY-MM-DD',HH,'") + strlen (type_string) + strlen ("',1.XXXXXXXXXXXXXXXXXXXXXXXXXXXXe+xxxxx'") + strlen (audit_type_string) + strlen ("',4AQI,'") + RESERVED_LEN + strlen ("')") + 1; //get length of query string
	if ((query = malloc (query_len)) == NULL) return NULL;
	if (replace == 0)
	{
		if (aqi != -2) ret = snprintf (query, query_len, \
		"INSERT IGNORE INTO %s_%s%4" PRIu16 "%02" PRIu16 " (day,hour,type,value,audit_type,aqi,reserved) VALUES ('%4" PRIu16 "-%02" PRIu16 "-%02" PRIu16 "',%" PRIu16 ",'%s',%s,'%s',%d,'%s')", site, HOUR_AVERAGE_TABLE_NAME_PREFIX, year, month, year, month, day, hour, type_string, value, audit_type_string, (int)aqi, reserved);
		else ret = snprintf (query, query_len, \
		"INSERT IGNORE INTO %s_%s%4" PRIu16 "%02" PRIu16 " (day,hour,type,value,audit_type,reserved) VALUES ('%4" PRIu16 "-%02" PRIu16 "-%02" PRIu16 "',%" PRIu16 ",'%s',%s,'%s','%s')", site, HOUR_AVERAGE_TABLE_NAME_PREFIX, year, month, year, month, day, hour, type_string, value, audit_type_string, reserved);
		if (ret < 0 || (size_t)(ret + 1) > query_len)
		{
			free (query);
			return NULL;
		}
	}
	else
	{
		if (aqi != -2) ret = snprintf (query, query_len, \
		"REPLACE INTO %s_%s%4" PRIu16 "%02" PRIu16 " (day,hour,type,value,audit_type,aqi,reserved) VALUES ('%4" PRIu16 "-%02" PRIu16 "-%02" PRIu16 "',%" PRIu16 ",'%s',%s,'%s',%d,'%s')", site, HOUR_AVERAGE_TABLE_NAME_PREFIX, year, month, year, month, day, hour, type_string, value, audit_type_string, (int)aqi, reserved);
		else ret = snprintf (query, query_len, \
		"REPLACE INTO %s_%s%4" PRIu16 "%02" PRIu16 " (day,hour,type,value,audit_type,reserved) VALUES ('%4" PRIu16 "-%02" PRIu16 "-%02" PRIu16 "',%" PRIu16 ",'%s',%s,'%s','%s')", site, HOUR_AVERAGE_TABLE_NAME_PREFIX, year, month, year, month, day, hour, type_string, value, audit_type_string, reserved);
		if (ret < 0 || (size_t)(ret + 1) > query_len)
		{
			free (query);
			return NULL;
		}
	}
	return query;
}


/*construct query string for hour average of PM25 PM10 from raw data,
 * return pointer to that string, caller should free it after
 *  using it, return NULL if error occur or failure
*/
static char * construct_PM10_PM25_1hour_retrieve_from_raw_table_mysql_query (uint16_t year, uint16_t month, uint16_t day, uint16_t hour, enum SITE sitetype, enum TYPE type)
{
	size_t query_len = 0;
	char *type_string;
	char *query_string;
	char *site;
	int ret;

	// if (check_year_month_day (year, month, day) || (type != PM10 && type != PM25) || hour > 24 || check_site (sitetype)) return NULL;
	site = get_site_name (sitetype);

	//if query hour a, then query minutes from (a-1):00:00 - (a-1):59:59
	if ((type_string = get_type_name(type)) == NULL) return NULL;

	query_len = strlen ("SELECT value FROM ") + strlen (site) + strlen ("_") + strlen (RAW_TABLE_NAME_PREFIX) + strlen ("yyyymmdd WHERE time='YYYY-MM-DD HH:MM:SS' AND type='") + strlen (type_string) + strlen ("'") + 1; //get length of query string
	if ((query_string = malloc (query_len)) == NULL) return NULL;
	if (hour == 24)
	{
		uint32_t tomorrow;
		if ((tomorrow = get_tomorrow (year, month, day)) == 0) return NULL;
		year = (uint16_t) (tomorrow / 10000);
		month = (uint16_t) (tomorrow % 10000) / 100;
		day = (uint16_t) (tomorrow % 100);
		if ((ret = snprintf (query_string, query_len, \
		"SELECT value FROM %s_%s%4" PRIu16 "%02" PRIu16 "%02" PRIu16 " WHERE time='%4" PRIu16 "-%02" PRIu16 "-%02" PRIu16" %02" PRIu16 ":%02" PRIu16 ":%02" PRIu16 "' AND type='%s'", \
	site, RAW_TABLE_NAME_PREFIX, year, month, day, year, month, day, 0, 0, 0, type_string)) < 0)
		{
			free (query_string);
			return NULL;
		}
	}
	else if ((ret = snprintf (query_string, query_len, \
	"SELECT value FROM %s_%s%4" PRIu16 "%02" PRIu16 "%02" PRIu16 " WHERE time='%4" PRIu16 "-%02" PRIu16 "-%02" PRIu16" %02" PRIu16 ":%02" PRIu16 ":%02" PRIu16 "' AND type='%s'", \
	site, RAW_TABLE_NAME_PREFIX, year, month, day, year, month, day, hour, 0, 0, type_string)) < 0)
	{
		free (query_string);
		return NULL;
	}
	if ((size_t)(ret + 1) > query_len)
	{
		free (query_string);
		return NULL;
	}
	return query_string;
}


/*construct query string for hour average that get raw data from database 
 * within an hour,return pointer to that string, caller should free it after
 *  using it, return NULL if error occur or failure
*/
static char * construct_1hour_rawdata_retrieve_mysql_query (uint16_t year, uint16_t month, uint16_t day, uint16_t hour, enum SITE sitetype, enum TYPE type)
{
	size_t query_len = 0;
	char *type_string;
	char *query_string;
	char *site;
	int ret;

	// if (check_year_month_day (year, month, day) || check_type_enum (type) || hour > 24 || check_site (sitetype)) return NULL;
	site = get_site_name (sitetype);

	//if query hour a, then query minutes from (a-1):00:00 - (a-1):59:59
	hour--;
	if ((type_string = get_type_name(type)) == NULL) return NULL;

	query_len = strlen ("SELECT time,value FROM ") + strlen (site) + strlen ("_") + strlen (RAW_TABLE_NAME_PREFIX) + strlen ("yyyymmdd WHERE time BETWEEN 'yyyy-mmmm-dd hh:mm:ss' AND 'yyyy-mmmm-dd hh:mm:ss' AND type='") + strlen (type_string) + strlen ("'") + 1; //get length of query string
	if ((query_string = malloc (query_len)) == NULL) return NULL;
	if ((ret = snprintf (query_string, query_len, \
	"SELECT time,value FROM %s_%s%4" PRIu16 "%02" PRIu16 "%02" PRIu16 " WHERE time BETWEEN '%4" PRIu16 "-%02" PRIu16 "-%02" PRIu16" %02" PRIu16 ":%02" PRIu16 ":%02" PRIu16 "' and '%4" PRIu16 "-%02" PRIu16 "-%02" PRIu16 " %02" PRIu16 ":%02" PRIu16 ":%02" PRIu16 "' AND type='%s'", \
	site, RAW_TABLE_NAME_PREFIX, year, month, day, year, month, day, hour, 0, 0, year, month, day, hour, 59, 59, type_string)) < 0)
	{
		free (query_string);
		return NULL;
	}
	if ((size_t)(ret + 1) > query_len)
	{
		free (query_string);
		return NULL;
	}
	return query_string;
}


/*
 * construct query string for hour average ranged from 1 to 24h
 * in a day,
 */
static char * construct_hour_average_from_1_to_24_mysql_query (uint16_t year, uint16_t month, uint16_t day, enum SITE sitetype, enum TYPE type)
{
	size_t query_len = 0;
	char *type_string;
	char *query_string;
	char *site;
	int ret;
	query_string = NULL;

	if (check_year_month_day (year, month, day) || check_site (sitetype) || check_type_enum (type)) return NULL;
	if ((type_string = get_type_name(type)) == NULL) return NULL;
	if ((site = get_site_name (sitetype)) == NULL) return NULL;

	query_len = strlen ("SELECT value,audit_type FROM ") + strlen (site) + strlen ("_") + strlen (HOUR_AVERAGE_TABLE_NAME_PREFIX) + strlen ("YYYYMM WHERE day='YYYY-MM-DD' AND hour between 1 AND 24 AND type='") + strlen (type_string) + strlen ("'") + 1; //get length of query string
	if ((query_string = malloc (query_len)) == NULL) return NULL;

	if ((ret = snprintf (query_string, query_len, "SELECT value,audit_type FROM %s_%s%" PRIu16 "%02" PRIu16 " WHERE day='%" PRIu16 "-%02" PRIu16 "-%02" PRIu16 "' AND hour between 1 AND 24 AND type='%s'", site, HOUR_AVERAGE_TABLE_NAME_PREFIX, year, month, year, month, day, type_string)) < 0 || (size_t)ret + 1 > query_len)
	{
		free (query_string);
		return NULL;
	}
	return query_string;
}

/*
 * construct O3 1h max insert string, replace indicate 
 * whether replace if already exist,
 */
char * construct_O3_1h_max_insert_mysql_query (uint16_t year, uint16_t month, uint16_t day, char *value, char *reserved_string, enum SITE sitetype, int replace)
{
	char *query;
	size_t len;
	char *reserved = "";
	int ret;
	char *site;
	int16_t aqi;

	/*if (check_year_month_day (year, month, day) || value == NULL || check_string_of_value (value) || check_site (sitetype)) return NULL;
	if (value[0] == '-' && strncmp (value, "-1", 3) && strncmp (value, "-1.0", 5)) return NULL;*/
	if (reserved_string != NULL) reserved == reserved_string;
	if ((site = get_site_name (sitetype)) == NULL) return NULL;
	aqi = calc_IAQI (IAQI_O3_1H, value);
	if (aqi == -1 || aqi == -3 || aqi == -4) return NULL;

	if (!replace) len = strlen ("INSERT IGNORE INTO ") + strlen (site) + strlen (O3_1H_MAX_TABLE_NAME_SUFFIX) + strlen (" (day,value,aqi,reserved) VALUES ('YYYY-MM-DD',1.XXXXXXXXXXXXXXXXXXXXXXXXXXXX+eXXXXX,XXX,'XXXXXXXXXXXXXXXXXXXX')") + 1;
	else len = strlen ("REPLACE INTO ") + strlen (site) + strlen (O3_1H_MAX_TABLE_NAME_SUFFIX) + strlen (" (day,value,aqi,reserved) VALUES ('YYYY-MM-DD',1.XXXXXXXXXXXXXXXXXXXXXXXXXXXX+eXXXXX,XXX,'XXXXXXXXXXXXXXXXXXXX')") + 1;
	if ((query = malloc (len)) == NULL) return NULL;
	if (!replace)
	{
		if (aqi != -2) ret = snprintf (query, len, "INSERT IGNORE INTO %s%s (day,value,aqi,reserved) VALUES ('%" PRIu16 "-%02" PRIu16 "-%02" PRIu16 "',%s, %d, '%s')", site, O3_1H_MAX_TABLE_NAME_SUFFIX, year, month, day, value, (int) aqi, reserved);
		else ret = snprintf (query, len, "INSERT IGNORE INTO %s%s (day,value,reserved) VALUES ('%" PRIu16 "-%02" PRIu16 "-%02" PRIu16 "',%s,'%s')", site, O3_1H_MAX_TABLE_NAME_SUFFIX, year, month, day, value, reserved);
		if (ret < 0)
		{
			free (query);
			return NULL;
		}
		if ((size_t)ret + 1 > len)
		{
			free (query);
			return NULL;
		}
	}
	else
	{
		if (aqi != -2) ret = snprintf (query, len, "REPLACE INTO %s%s (day,value,aqi,reserved) VALUES ('%" PRIu16 "-%02" PRIu16 "-%02" PRIu16 "',%s,%d,'%s')", site, O3_1H_MAX_TABLE_NAME_SUFFIX, year, month, day, value, (int) aqi, reserved);
		else ret = snprintf (query, len, "REPLACE INTO %s%s (day,value,reserved) VALUES ('%" PRIu16 "-%02" PRIu16 "-%02" PRIu16 "',%s,'%s')", site, O3_1H_MAX_TABLE_NAME_SUFFIX, year, month, day, value, reserved);
		if (ret < 0)
		{
			free (query);
			return NULL;
		}
		if ((size_t)ret + 1 > len)
		{
			free (query);
			return NULL;
		}
	}
	return query;
}

/*
 * query o3 8h moving average max from each moving average
 * of 8 hours, return 0 if successfully got, return 1 if
 * not enough valid moving average(14), return 2 if valid 
 * moving average less than 14,but max of them greater 
 * than limited value,return 3 if invalid moving average 
 * exist, return 4 if valid hour average total count greater
 * than 17,return 5 if get some moving average failed,return 
 * -1 if arg invalid, return other negative integer if other
 *  err occur
 */
int query_O3_8h_moving_average_max_from_moving_average (char *query, char *value, MYSQL *mysql)
{
	MYSQL_RES *result;
	MYSQL_ROW row;
	my_ulonglong num_rows;
	int valid_hours, ret;
	
	//if (query == NULL || value == NULL) return -1;
	strncpy (value, "-99999", 7);
	if (mysql_real_query (mysql, query, strlen (query))) return -2;
	if ((result = mysql_store_result (mysql)) == NULL)
	{
		return -3;
	}
	if ((num_rows = mysql_num_rows (result)) > 17)
	{
		mysql_free_result (result);
		return 4;
	}
	if (num_rows == 0)
	{
		strncpy (value, "-1", 3); //*value = -1;
		mysql_free_result (result);
		return 1;
	}
	if (num_rows < 0)
	{
		mysql_free_result (result);
		return -4;
	}
	valid_hours = (int) num_rows;
	my_ulonglong i;
	for (i = 0; i < num_rows; i++)
	{
		//double moving;
		if ((row = mysql_fetch_row (result)) == NULL)
		{
			mysql_free_result (result);
			return 5;
		}
		if (check_string_of_value (row[0]))
		{
			mysql_free_result (result);
			return 3;
		}
		//moving = atof (row[0]);
		if (!strncmp (row[0], "-1", 3) || !strncmp (row[0], "-1.0", 5))
		{
			valid_hours--;
			continue;
		}
		if (row[0][1] == '-')
		{
			mysql_free_result (result);
			return 3;
		}
		if ((ret = compare_string_number (value, row[0])) == -1)
		{
			if ((ret = snprintf (value, STRING_NUMBER_BUFSIZ, "%s", row[0])) < 0 || ret + 1 > STRING_NUMBER_BUFSIZ) return -5;
		}
		else if (ret == -INT_MAX) return -6;

	}
	mysql_free_result (result);
	if (!strncmp (value, "-99999", 7))
	{
		strncpy (value, "-1", 3); //*value = -1;
		return 1;
	}
	if (valid_hours < 14)
	{
		if ((ret = compare_string_number (value, IAQI_100_O3_8H)) == 1) return 2;
		else if (ret == -INT_MAX )return -7;
	}
	//*value = max;
	return 0;
}

/*
 * get O3 8h moving average value from hour average table 
 * and calc,return 0 if successfully get an normal value,
 * return 1 if valid hours less than 6, return 2 if invalid
 * hour average value or audit exist, return 3 if more than 8 hour 
 * average values exist in given 8 hour,return -1 if arg 
 * invalid, return other negative value if some err occur
 */
int query_O3_8h_moving_average_from_hour_average (char *query, char *value, MYSQL *mysql)
{
	MYSQL_RES *result;
	MYSQL_ROW row;
	my_ulonglong num_rows;
	int valid_hours;
	//double sum;
	int ret;
	/*if (query == NULL || value == NULL || mysql == NULL)
	{
		return -1;
	}*/

	if (mysql_real_query (mysql, query, strlen (query)))
	{
		printf ("query:%s\n",query);
		return -2;
	}
	if ((result = mysql_store_result (mysql)) == NULL)
	{
		return -3;
	}
	num_rows = mysql_num_rows (result);
	if (num_rows > 8)
	{
		mysql_free_result (result);
		return 3;
	}
	if (num_rows < 6 && num_rows >= 0)
	{
		mysql_free_result (result);
		strncpy (value, "-1", 3); //value = -1;
		return 1;
	}
	if (num_rows < 0)
	{
		mysql_free_result (result);
		return -4;
	}
	my_ulonglong i;
	value[0] = '0';
	value[1] = '\0';
	for (i = 0, valid_hours = num_rows; i < num_rows; i++)
	{
		//double hour_value;
		enum AUDIT_TYPE hour_audit;
		if ((row = mysql_fetch_row (result)) == NULL)
		{
			mysql_free_result (result);
			return -5;
		}
		if (check_string_of_value (row[0]) || (hour_audit = get_audit_type (row[1], 1)) == invalid_audit_type)
		{
			mysql_free_result (result);
			return 2;
		}
		//hour_value = atof (row[0]);
		if (check_value_audit_match (row[0], hour_audit))
		{
			mysql_free_result (result);
			return 2;
		}
		if (!strncmp (row[0], "-1", 3) || !strncmp (row[0], "-1.0", 5))
		{
			valid_hours--;
			continue;
		}
		if (add_string_number (value, row[0], value, STRING_NUMBER_BUFSIZ))
		{
			return -6;//sum += hour_value;
		}
	}
	mysql_free_result (result);
	if (valid_hours < 6)
	{
		strncpy (value, "-1", 3); //*value = -1;
		return 1;
	}
	if (divide_string_number (value, valid_hours, DATA_ROUNDING_BIT1, value, STRING_NUMBER_BUFSIZ))
	{
		return -7; //*value = sum / valid_hours;
	}
	return 0;
}

/*
 * issue a query for data within an given hour and process the data, return 0 if
 * validity satisfied and value is a normal data,return 2 if data less than 0 but
 * greater than specified lower limit,return 1 if valid minutes less than 45 
 * and value is -1 ,return -1 if arg invalid, return -2 if some err occur, 
*/
static int query_1hour_rawdata_and_calc_hour_average (char *query, enum TYPE type, char *value, unsigned bytes, MYSQL *mysql)
{
	MYSQL_RES *result;
	MYSQL_ROW row;
	my_ulonglong values_num;
	my_ulonglong valid_num;
	my_ulonglong minute_num;
	int ret;
	//char sum[STRING_NUMBER_BUFSIZ];
	value[0] = '0';
	value[1] = '\0';
	//double sum = 0;

	// if (value == NULL || bytes < STRING_NUMBER_BUFSIZ || mysql == NULL || query == NULL) return -1;

	if (mysql_real_query (mysql, query, strlen (query)))
	{
		return -2;
	}
	if ((result = mysql_store_result (mysql)) == NULL)
	{
		mysql_free_result (result);
		return -2; //return 2 if store result failure
	}

	values_num = mysql_num_rows (result);
	valid_num = values_num;
	if (values_num < 45)
	{
		strncpy (value, "-1", 3); //value = -1;
		mysql_free_result (result);
		return 1; //return 1 if valid data in an hour less that 45 minutes
	}
	my_ulonglong i;
	char last_minute[2]; //minute of latest processing row in for
	for (minute_num = 0, last_minute[0] = last_minute[1] = '\0',i = 1; i <= values_num; i++)
	{
		unsigned long *lengths;
		int sign = 1; //1 if positive, -1 if negative
		char *pos;
		if ((row = mysql_fetch_row (result)) == NULL)
		{
			fprintf (stderr, "fetch row failed or now more row exist, mysql_errno:%u, query string : %s\n", mysql_errno(mysql), query);
			mysql_free_result (result);
			return -2;
		}
		if ((lengths = mysql_fetch_lengths (result)) == NULL)
		{
			mysql_free_result (result);
			return -2;
		}
		if (check_string_of_time_from_mysql (row[0]))
		{
			valid_num--;
			continue;
		}
		int tmp;
		if ((tmp = check_string_of_value (row[1])))
		{
			valid_num--;
			continue;
		}
		if (last_minute[0] != row[0][14] || last_minute[1] != row[0][15])
		{
			last_minute[0] = row[0][14];
			last_minute[1] = row[0][15];
			minute_num++;
		}
		//sum += atof (row[1]);
		
		if ((ret = add_string_number (value, row[1], value, STRING_NUMBER_BUFSIZ)))
		{
			printf ("add err ret: %d,value:%s!,row:%s!\n",ret,value,row[1]);
			return -2;
		}
	}
	mysql_free_result (result);
	if (valid_num < values_num) fprintf (stderr, "invalid time exist in given minute from raw table, query string : %s\n,", query);
	if (minute_num < 45)
	{
		strncpy (value, "-1", 3); //value = -1;
		return 1; //
	}
	if (divide_string_number (value, valid_num, DATA_FRACTION_LEN / 2, value, STRING_NUMBER_BUFSIZ))
	{
		printf ("divide err,value:%s!,valid_num:%ld\n", value,valid_num);
		return -2;//else *value = sum / valid_num;
	}
	switch (type)
	{
		case O3: case SO2: case NO2: case PM10: case PM25:
		if ((ret = our_string_rounding (value, STRING_NUMBER_BUFSIZ, 3)))
		{
			return -2;
		}
		if ((ret = rounding_data (value, type)) != 0 && ret != 1)
		{
			printf ("ret:%d,value now : %s\n", ret, value);
			return -2;
		}

		break;
		case CO:
		if (our_string_rounding (value, STRING_NUMBER_BUFSIZ, 1))
		{
		printf ("err value is %s!\n",value);
			return -2;
		}
		if ((ret = rounding_data (value, type)) != 0 && ret != 1)
		{
			return -2;
		}
		break;
	}
	if (ret == 0) return 2;
	else return 0;
}

/*
 * issue a query for hour average of pm25 and pm10 from raw table,
 * return 0 if success, return -1 if arg 
 * invalid,return 1 if no data exist, return 2 if more than one result
 * exist, return 3 if data invalid,return -2 if other err
 */
static int query_PM10_PM25_hour_average_from_raw (char *query, char *value, unsigned int bytes, MYSQL *mysql)
{
	MYSQL_RES *result;
	MYSQL_ROW row;
	my_ulonglong values_num;
	int ret;

	//if (value == NULL || mysql == NULL || query == NULL || bytes < STRING_NUMBER_BUFSIZ) return -1;

	if (mysql_real_query (mysql, query, strlen (query)))
	{
		printf ("query:%s\n",query);
		return -2;
	}
	if ((result = mysql_store_result (mysql)) == NULL)
	{
		mysql_free_result (result);
		return -2; //return 2 if store result failure
	}

	values_num = mysql_num_rows (result);
	if (values_num == 0)
	{
		mysql_free_result (result);
		return 1;
	}
	if (values_num > 1)
	{
		mysql_free_result (result);
		return 2;
	}
	if (values_num < 0)
	{
		mysql_free_result (result);
		return -2;
	}
	if ((row = mysql_fetch_row (result)) == NULL)
	{
		mysql_free_result (result);
		return -2;
	}
	if (check_string_of_value (row[0]))
	{
		mysql_free_result (result);
		return 3;
	}
	for (int i = 0; i <= strlen (row[0]); i++) value[i] = row[0][i];
	if (our_string_rounding (value, STRING_NUMBER_BUFSIZ, 3))
	{
		return -2;
	}
	mysql_free_result (result);
	return 0;
}

/*
 * return 0 if queried and calc and get an normal value,
 * return 1 if not enough valid hour average data exist,
 * return 2 if some hour average data or audit invalid,
 * return -1 if arg invalid, return other negative 
 * integer if some err occur
 */
int query_hour_average_from_1_to_24 (char *query, enum TYPE type, char *value, MYSQL *mysql)
{
	MYSQL_RES *result;
	MYSQL_ROW row;
	my_ulonglong num_rows;
	char sum[STRING_NUMBER_BUFSIZ];
	int valid_hours;
	int ret;

	// if (query == NULL || value == NULL || mysql == NULL) return -1;

	if (mysql_real_query (mysql, query, strlen (query)))
	{
		return -2;
	}
	if ((result = mysql_store_result (mysql)) == NULL) return -3;
	if ((num_rows = mysql_num_rows (result)) > 24)
	{
		mysql_free_result (result);
		return 2;
	}
	else if (num_rows < 20 && num_rows >= 0)
	{
		mysql_free_result (result);
		//value = -1;
		value[0] = '-';
		value[1] = '1';
		value[2] = '\0';
		return 1;
	}
	else if (num_rows < 0)
	{
		mysql_free_result (result);
		return -4;
	}

	sum[0] = '0';
	sum[1] = '\0';
	valid_hours = num_rows;
	for (my_ulonglong hour = 1 ; hour < 24; hour++)
	{
		//double hour_value;
		enum AUDIT_TYPE audit;
		if ((row = mysql_fetch_row (result)) == NULL)
		{
			mysql_free_result (result);
			return -5;
		}
		if (check_string_of_value (row[0]))
		{
			mysql_free_result (result);
			return 2;
		}
		if ((audit = get_audit_type (row[1], 1)) == invalid_audit_type)
		{
			mysql_free_result (result);
			return 2;
		}
		//hour_value = atof (row[0]);
		if (check_value_audit_match (row[0], audit)) return 2;
		if (row[0][0] == '-' &&  row[0][1] == '1' && (row[0][2] == '\0' || (row[0][2] == '.' && row[0][3] == '0' && row[0][4] == '\0'))) //value == -1
		{
			valid_hours--;
			continue;
		}
		if (add_string_number (sum, row[0], sum, STRING_NUMBER_BUFSIZ))
		{
			return -6; //sum += hour_value;
		}
	}
	if (valid_hours < 20)
	{
		//*value = -1;
		value[0] = '-';
		value[1] = '1';
		value[2] - '\0';
		return 1;
	}
	if (divide_string_number (sum, (int) valid_hours, DATA_ACCURACY_BIT, value, STRING_NUMBER_BUFSIZ))
	{
		return -7; //value = sum / valid_hours;
	}
	switch (type)
	{
		case SO2: case NO2: case O3: case PM10: case PM25:
		if (our_string_rounding (value, STRING_NUMBER_BUFSIZ, DATA_ROUNDING_BIT1))
		{
			return -8;
		}
		break;
		case CO:
		if (our_string_rounding (value, STRING_NUMBER_BUFSIZ, DATA_ROUNDING_BIT2))
		{
			return -8;
		}
		break;
	}
	return 0;
}

/*
 * query O3 1h max from 24hour average, return 0 if successfully
 * get, return 1 if invalid hour average value or auditexist, 
 * return 2 if more than 24 hour average value exist, return 1
 * if no value exist or just -1 exist, return -1 if arg invalid, 
 * return other negative interger if other err
 */
int query_O3_1h_max_from_hour_average (char *query, char *value, MYSQL *mysql)
{
	MYSQL_RES *result;
	MYSQL_ROW row;
	my_ulonglong num_rows;
	int ret;
	size_t max_len;
	//if (query == NULL || value == NULL) return -1;
	max_len = strlen ("-99999") + 1;
	if ((ret = snprintf (value, max_len, "%s", "-99999")) < 0 || ret + 1 > max_len) return -2;

	if (mysql_real_query (mysql, query, strlen (query))) return -2;
	if ((result = mysql_store_result (mysql)) == NULL) return -3;
	if ((num_rows = mysql_num_rows (result)) > 24)
	{
		mysql_free_result (result);
		return 2;
	}
	if (num_rows < 0)
	{
		mysql_free_result (result);
		return -3;
	}
	if (num_rows == 0) return 1;

	my_ulonglong i;
	for (i = 0; i < num_rows; i++)
	{
		double hour_value, hour_audit;
		if ((row = mysql_fetch_row (result)) == NULL)
		{
			mysql_free_result (result);
			return -4;
		}
		if (check_string_of_value (row[0]))
		{
			mysql_free_result (result);
			return 3;
		}
		hour_audit = get_audit_type (row[1], 1);
		if (hour_audit == invalid_type)
		{
			mysql_free_result (result);
			return 3;
		}
		//hour_value = atof (row[0]);
		if (check_value_audit_match (row[0], hour_audit))
		{
			mysql_free_result (result);
			return 3;
		}
		if (!strncmp (row[0], "-1", 3) || !strncmp (row[0], "-1.0", 5)) continue; //if (hour_value == -1) continue;
		if ((ret = compare_string_number (value, row[0])) == -1)
		{
			if ((ret = snprintf (value, STRING_NUMBER_BUFSIZ, "%s", row[0])) < 0 || ret + 1 >  STRING_NUMBER_BUFSIZ) return -5;
		}	//if (max < hour_value) max = hour_value;
		else if (ret == -INT_MAX) return -6;
	}
	mysql_free_result (result);
	if (!strncmp (value, "-1", 3) || !strncmp (value, "-1.0", 5)) //max == -1
	{
		return 1;
	}
	if (!strncmp (value, "-99999", max_len))
	{
		strncpy (value, "-1", 3);
		return 1;
	}
	return 0;
}

/*
 * stat hour average from raw data, return 0 if ok, return 3
 * if data invalid, return 2 if data more than one data 
 * exist(PM10,PM25), return 1 if no data exist(PM10,PM25),return -1 
 * if arg invalid, return other negative integer if some error occur,
 * other if failed
 */
int stat_hour_average (uint16_t year, uint16_t month, uint16_t day, uint16_t hour, enum TYPE type, char *reserved_string, enum SITE sitetype, MYSQL *mysql)
{
	char *query;
	char *reserved = "";
	int ret;
	//double value;
	char value[STRING_NUMBER_BUFSIZ];
	enum AUDIT_TYPE audit = empty_audit;
	char *site;
	int rounding_err = 0;

	//if (check_year_month_day (year, month, day) || hour > 24 || hour == 0 || check_site (sitetype) || check_type_enum (type) || mysql == NULL) return -1;
	if ((site = get_site_name(sitetype)) == NULL) return -1;

	if (type == PM25 || type == PM10) 
	{
		if ((query = construct_PM10_PM25_1hour_retrieve_from_raw_table_mysql_query (year, month, day, hour, sitetype, type)) == NULL) return -2;
		if ((ret = query_PM10_PM25_hour_average_from_raw (query, value, STRING_NUMBER_BUFSIZ, mysql)) == 2)
		{
			free (query);
			return 2;
		}
		free (query);
		if (ret == 3) return 3;
		if (ret == 1)
		{
			strncpy (value, "-1", 3);
			//value = -1;
			audit = H;
		}
		if (ret == -1) return -1;
		if (ret && ret != 1)
		{
			return -3;
		}
		//now data is available, first check and then, maybe need rounding
		if ((ret = rounding_data (value, type)) == 2 || ret == -1) return 3;
		if (ret == 0) audit = T;
	}
	else
	{
		//construct rawdata retrieve mysql query string
		if ((query = construct_1hour_rawdata_retrieve_mysql_query (year, month, day, hour, sitetype, type)) == NULL) return -2;
		//retrieve raw data from mysql and process and calc average
		if ((ret = query_1hour_rawdata_and_calc_hour_average (query, type, value, STRING_NUMBER_BUFSIZ, mysql)) != 1 && ret != 0 && ret != 2)
		{
			free (query);
			return -3;
		}
		free (query);
		if (ret == 1) audit = H;
		else if (ret == 2) audit = T;
		//now data is available, first check and then, maybe need rounding
		//else if ((ret = rounding_data (value, type)) == 2) return 3;
		//else if (ret == 0) audit = T;
		eliminate_fraction_string_trailing_zero (value);
		
	}
	if (create_table_hour_average (year, month, sitetype, mysql)) return -9;
	//lock hour average table
	char *lock;
	if ((lock = construct_hour_average_table_lock_mysql_query (year, month, sitetype)) == NULL) return -5;
	if (mysql_real_query (mysql, lock, strlen (lock)))
	{
		free (lock);
		return -6;
	}
	free (lock);

	//construct hour average insert mysql string
	char s[20];
	/*if (type == CO) if (our_string_rounding (&value, 1)) return -10;
	else if (our_string_rounding (&value, 3)) return -10;*/
	if ((query = construct_hour_average_insert_mysql_query (year, month, day, hour, type, value, audit, "", sitetype, 1)) == NULL)
	{
		mysql_real_query (mysql, "UNLOCK TABLES", strlen ("UNLOCK TABLES"));
		return -7;
	}
	//insert into mysql
	if (mysql_real_query (mysql, query, strlen (query)))
	{
		mysql_real_query (mysql, "UNLOCK TABLES", strlen ("UNLOCK TABLES"));
		free (query);
		return -8;
	}
	mysql_real_query (mysql, "UNLOCK TABLES", strlen ("UNLOCK TABLES"));
	free (query);
	return 0;
}

/*
 * audit hour average value, return 0 if successfully replace original
 * data, return 1 if there is originally data in the hour average
 * table,return -1 if arg invalid, return other negative value if
 * other err occur
 */
int audit_hour_average (uint16_t year, uint16_t month, uint16_t day, uint16_t hour, enum TYPE type, enum AUDIT_TYPE audit, char *reserved_string, enum SITE sitetype, MYSQL *mysql)
{
	char *site;
	char *query;
	//if (check_year_month_day (year, month, day) || hour == 0 || hour > 24 || check_type_enum (type) || check_site (sitetype) || mysql == NULL) return -1; //arg invalid
	if ((site = get_site_name (sitetype)) == NULL) return -1;
	char *reserved = "";
	if (reserved_string != NULL) reserved = reserved_string;
	//audit type should be one of the following types
	switch (audit)
	{
		case B: case BB: case W: case H: case HSp: case LSp: case PZ: case PS: case AS: case CZ: case CS:
		break;
		default:
		return -1;
	}

	if (create_table_hour_average (year, month, sitetype, mysql)) return -6;

	//lock hour average table
	char *lock;
	if ((lock = construct_hour_average_table_lock_mysql_query (year, month, sitetype)) == NULL) return -2;
	if (mysql_real_query (mysql, lock, strlen (lock)))
	{
		free (lock);
		return -3;
	}
	free (lock);

	//construct hour average insert mysql string
	char value[3] = "-1";
	if ((query = construct_hour_average_insert_mysql_query (year, month, day, hour, type, value, audit, reserved, sitetype, 1)) == NULL)
	{
		mysql_real_query (mysql, "UNLOCK TABLES", strlen ("UNLOCK TABLES"));
		return -4;
	}
	//insert into mysql
	if (mysql_real_query (mysql, query, strlen (query)))
	{
		mysql_real_query (mysql, "UNLOCK TABLES", strlen ("UNLOCK TABLES"));
		free (query);
		return -5;
	}
	free (query);
	my_ulonglong affected = mysql_affected_rows (mysql);
	//unlock table
	mysql_real_query (mysql, "UNLOCK TABLES", strlen ("UNLOCK TABLES"));
	if (affected == 1) return 1;

	return 0;
}

/*
 * stat day average value and data,and insert into day
 * average table,return 3 if some hour average invalid,return
 * 2 if get some hour average failed,return 1 if valid
 * hours less than 20, return -1 if arg invalid, return other
 * negative interger if other err occur
 */
int stat_day_average (uint16_t year, uint16_t month, uint16_t day, enum TYPE type, char *reserved_string, enum SITE sitetype, MYSQL *mysql)
{
	char *reserved;
	char *query;
	int ret;
	//if (check_year_month_day (year, month, day) || mysql == NULL) return -1;
	switch (type)
	{
		case SO2: case NO2: case CO: case PM10: case PM25:
		break;
		default:
		return -1;
	}
	if (reserved_string != NULL) reserved = reserved_string;

	char value[STRING_NUMBER_BUFSIZ];
	//construct hour average query string
	if ((query = construct_hour_average_from_1_to_24_mysql_query (year, month, day, sitetype, type)) == NULL) return -2;

	ret = query_hour_average_from_1_to_24 (query, type, value, mysql);
	free (query);
	if (ret == 2) return 3;
	else if (ret < 0) return -3;

	//check or create day average table
	if (create_table_day_average (sitetype, mysql)) return -7;

	//lock day average table
	if ((query = construct_day_average_table_lock_mysql_query (sitetype)) == NULL) return -6;
	if (mysql_real_query (mysql, query, strlen (query)))
	{
		free (query);
		return -6;
	}
	free (query);

	//construct day average insert string
	if ((query = construct_insert_day_average_mysql_query (year, month, day, type, value, reserved, sitetype, 1)) == NULL) return -4;

	//issue insert query
	if (mysql_real_query (mysql, query, strlen (query)))
	{
		mysql_real_query (mysql, "UNLOCK TABLES", strlen ("UNLOCK TABLES"));
		free (query);
		return -5;
	}
	mysql_real_query (mysql, "UNLOCK TABLES", strlen ("UNLOCK TABLES"));
	free (query);
	return 0;
}

/*
 * stat O3 1h max return 0 if query o3 1h max table 
 * success,return 3 if some hour average invalid,
 * return  4 if create table failed, return 2 if get 
 * some hour average failed,return 1 if no value exist
 * in hour average data, return -1 if arg invalid, return other
 * negative interger if other err occur, return 5 if more
 * than 24 hour average value exist
 */
int stat_O3_1h_max (uint16_t year, uint16_t month, uint16_t day, enum SITE sitetype, MYSQL *mysql)
{
	int ret;
	char *query;
	//if (check_year_month_day (year, month, day) || check_site (sitetype) || mysql == NULL) return -1; //arg invalid

	char max[STRING_NUMBER_BUFSIZ];
	//construct 24 hour query string
	if ((query = construct_hour_average_from_1_to_24_mysql_query (year, month, day, sitetype, O3)) == NULL) return -2;
	ret = query_O3_1h_max_from_hour_average (query, max, mysql);
	free (query);
	if (ret == 3) return 3;
	if (ret == 2) return 5;
	if (ret && ret != 1) return -2;

	//check table exist or create
	if (create_table_O3_1h_max (sitetype, mysql)) return 4;

	//lock table
	if ((query = construct_O3_1h_max_table_lock_mysql_query (sitetype)) == NULL) return -5;
	if (mysql_real_query (mysql, query, strlen (query)))
	{
		free (query);
		return -5;
	}
	free (query);
	
	//construct O3 1h max insert mysql query string
	if ((query = construct_O3_1h_max_insert_mysql_query (year, month, day, max, NULL, sitetype, 1)) == NULL)
	{
		mysql_real_query (mysql, "UNLOCK TABLES", strlen ("UNLOCK TABLES"));
		return -3;
	}

	//issue insert query
	if (mysql_real_query (mysql, query, strlen (query)))
	{
		mysql_real_query (mysql, "UNLOCK TABLES", strlen ("UNLOCK TABLES"));
		free (query);
		return -4;
	}
	mysql_real_query (mysql, "UNLOCK TABLES", strlen ("UNLOCK TABLES"));
	free (query);
	return 0;

}

/*
 * get O3 8h from hour average data, return 0 
 * if successfully get an normal result,return 1
 * if valid hours less than 6,return 2 if morethan
 * 8 result of hour average value exist,return 3 if exist
 * invalid hour average value, return 4 if insert failed,
 * return -1 if arg invalid, return other negative integer 
 * if some err occur
 */
int stat_O3_8h_moving_average(uint16_t year, uint16_t month, uint16_t day, uint16_t hour, enum SITE sitetype, MYSQL *mysql)
{
	int ret;
	char *query;
	//check arg
	//if (check_year_month_day (year, month, day) || hour > 24 || hour == 0 || check_site (sitetype) || mysql == NULL) return -1;

	//construct hour average query string
	
	if ((query = construct_O3_8h_moving_average_8_hours_mysql_query (year, month, day, hour, sitetype)) == NULL) return -2;
	
	//get hour average and check and calc
	char value[STRING_NUMBER_BUFSIZ];
	ret = query_O3_8h_moving_average_from_hour_average (query, value, mysql);
	free (query);
	if (ret == 2) return 3;
	if (ret == 3) return 2;
	if (ret < 0)
	{
		return -3;
	}
	if (ret && ret != 1)
	{
		return -3;
	}

	//check table exist or create
	if (create_table_O3_8h_moving_average (year, month, sitetype, mysql)) return -7;

	//lock table
	if ((query = construct_O3_moving_average_table_lock_mysql_query	(sitetype, year, month)) == NULL) return -4;
	if (mysql_real_query (mysql, query, strlen (query)))
	{
		free (query);
		return -5;
	}
	free (query);

	//construct insert mysql query
	if ((query = construct_O3_8h_moving_average_insert_string (year, month, day, hour, value, NULL, sitetype, 1)) == NULL)
	{
		mysql_real_query (mysql, "UNLOCK TABLES", strlen ("UNLOCK TABLES"));
		return -6;
	}
	if (mysql_real_query (mysql, query, strlen (query)))
	{
		mysql_real_query (mysql, "UNLOCK TABLES", strlen ("UNLOCK TABLES"));
		free (query);
		return 4;
	}
	mysql_real_query (mysql, "UNLOCK TABLES", strlen ("UNLOCK TABLES"));
	free (query);
	return 0;
}

/*
 * get O3 8h moving average max from moving average table and
 * get the max, return 0 if successfully get normal value, return 
 * 2 if invalid moving average exist and value is set, return 3 if 
 * count of moving average greater than 17,return 4 if get some 
 * moving average failed,return 5 if create o3 8h moving average 
 * max table failed, return -1 if arg invalid, return other 
 * negative integer if other err occur
 */
int stat_O3_8h_moving_average_max (uint16_t year, uint16_t month, uint16_t day, enum SITE sitetype, MYSQL *mysql)
{
	int ret;
	char *query;
	char value[STRING_NUMBER_BUFSIZ];

	//if (check_year_month_day (year, month, day) || check_site (sitetype) || mysql == NULL) return -1;

	//construct O3 8h moving average max from moving average query string and query
	if ((query = construct_O3_8h_moving_average_max_from_moving_average_mysql_query (year, month, day, sitetype)) == NULL) return -2;
	ret = query_O3_8h_moving_average_max_from_moving_average (query, value, mysql);
	free (query);
	if (ret == 3) return 2;
	if (ret == 4) return 3;
	if (ret == 5) return 4;
	if (ret != 0 && ret != 1 && ret != 2)
	{
		return -3;
	}

	//check table exist or create
	if (create_table_O3_8h_moving_average_max (sitetype, mysql)) return 5;

	//lock table
	if ((query = construct_O3_moving_average_max_table_lock_mysql_query (sitetype)) == NULL) return -4;
	if (mysql_real_query (mysql, query, strlen (query)))
	{
		free (query);
		return -5;
	}
	free (query);

	//construct insert string
	if ((query = construct_O3_8h_moving_average_max_insert_string (year, month, day, value, NULL, sitetype, 1)) == NULL)
	{
		mysql_real_query (mysql, "UNLOCK TABLES", strlen ("UNLOCK TABLES"));
		return -6;
	}
	if (mysql_real_query (mysql, query, strlen (query)))
	{
		free (query);
		mysql_real_query (mysql, "UNLOCK TABLES", strlen ("UNLOCK TABLES"));
		return -7;
	}
	free (query);
	return 0;
}
