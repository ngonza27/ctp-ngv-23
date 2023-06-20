#include <stdio.h>
#include <stdlib.h>
#include <linux/can/raw.h>
#include <string.h>

char* decode_dtc(unsigned int *data);

char* service_two(__u8 *data) {
	char* freeze_dtc = malloc(6);
	unsigned int dtc = (data[3]<<8) | data[4];
	char* decoded_dtc = decode_dtc(&dtc);
	strncpy(freeze_dtc, decoded_dtc, sizeof(freeze_dtc)-1);
	free(decoded_dtc);
	return freeze_dtc;
}