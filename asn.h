#include "Pdu.h"
#include "MonitorDataPacket.h"

e_type get_asn_type_from_name (char *name);

e_site get_asn_site_type_from_name (char *sitename);

char *encode_asn_type (void *type, asn_TYPE_descriptor_t *desc, long *enclen);

int write_out (const void *buffer, size_t size, void *app_key);
