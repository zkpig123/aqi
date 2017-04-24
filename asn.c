#include "asn.h"

e_type get_asn_type_from_name (char *name)
{
	if (name == NULL) return type_invalid_type;
	if (!strncmp (name, "SO2", strlen ("SO2"))) return type_so2;
	if (!strncmp (name, "NO2", strlen ("NO2"))) return type_no2;
	if (!strncmp (name, "O3", strlen ("O3"))) return type_o3;
	if (!strncmp (name, "CO", strlen ("CO"))) return type_co;
	if (!strncmp (name, "PM10", strlen ("PM10"))) return type_pm10;
	if (!strncmp (name, "PM25", strlen ("PM25"))) return type_pm25;
	if (!strncmp (name, "风速", strlen ("风速"))) return type_wind_velocity;
	if (!strncmp (name, "温度", strlen ("温度"))) return type_ambient_temperature;
	if (!strncmp (name, "湿度", strlen ("湿度"))) return type_humidity;
	if (!strncmp (name, "气压", strlen ("气压"))) return type_atom_pressure;
	if (!strncmp (name, "风向", strlen ("风向"))) return type_wind_direction;
	if (!strncmp (name, "室温", strlen ("室温"))) return type_room_temperature;
	if (!strncmp (name, "光强值", strlen ("光强值"))) return type_doas_light_insensity;
	if (!strncmp (name, "是否监测", strlen ("是否监测"))) return type_doas_monitor_status;
	if (!strncmp (name, "积分时间", strlen ("积分时间"))) return type_doas_integral_time;
	if (!strncmp (name, "平均次数", strlen ("平均次数"))) return type_doas_average_count;
	if (!strncmp (name, "光程", strlen ("光程"))) return type_doas_optical_path;
	if (!strncmp (name, "SO2信噪比", strlen ("SO2信噪比"))) return type_doas_so2_sig_noise_ratio;
	if (!strncmp (name, "SO2信噪比", strlen ("SO2信噪比"))) return type_doas_so2_sig_noise_ratio;
	if (!strncmp (name, "O3信噪比", strlen ("O3信噪比"))) return type_doas_o3_sig_noise_ratio;
	if (!strncmp (name, "信噪比", strlen ("信噪比"))) return type_doas_sig_noise_ratio;
	if (!strncmp (name, "PDA状态", strlen ("PDA状态"))) return type_doas_pda_status;
	if (!strncmp (name, "电机状态", strlen ("电机状态"))) return type_doas_elec_machine_status;
	if (!strncmp (name, "校准池位置", strlen ("校准池位置"))) return type_doas_adjust_poll_pos;
	return type_invalid_type;
}

e_site get_asn_site_type_from_name (char *sitename)
{
	if (!strncmp (sitename, "tyg", strlen ("tyg"))) return site_tyg;
	if (!strncmp (sitename, "tsg", strlen ("tsg"))) return site_tsg;
	return site_invalid_site;
}

/*
 * caller need to check the arguments
 */
char *encode_asn_type (void *type, asn_TYPE_descriptor_t *desc, long *enclen)
{
	char *enc;
	long len;
	char *ch;
	asn_enc_rval_t ec;
	char errbuf[128];
	size_t errlen = sizeof (errbuf);

	if (asn_check_constraints (desc, type, errbuf, &errlen)) return NULL;
	ec = der_encode (desc, type, NULL, (void*)0);
	if (ec.encoded == -1) return NULL;
	len = (long) ec.encoded;
	if ((enc = calloc (1, len)) == NULL) return NULL;
	ch = enc;
	ec = der_encode (desc, type, write_out, &ch);
	if (ec.encoded == -1) return NULL;
	*enclen = len;
	return enc;
}

int write_out (const void *buffer, size_t size, void *app_key)
{
	char **ch = app_key;
	memcpy (*ch, buffer, size);
	*ch += size;
	return 0;
}
