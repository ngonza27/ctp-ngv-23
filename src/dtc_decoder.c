#include <stdio.h>
#include <stdlib.h>
#include <linux/can/raw.h>

/**
 	@brief Decodes the hex value into readable DTC code
	@param dtc hex number of the reported code
	@return decoded DTC
*/
char* decode_dtc(unsigned int *dtc) {
  unsigned int category =  (*dtc >> 14) & 0x03;
	unsigned int is_sae_defined = (*dtc >> 12) & 0x01;
	unsigned int number = *dtc & 0xFFF;
	char* dtc_char = (char*)malloc(6 * sizeof(char));
	switch (category) {
		case 0:
			sprintf(dtc_char, "P%d%3X", is_sae_defined,number);
			break;
		case 1:
			sprintf(dtc_char, "C%d%3X", is_sae_defined,number);
			break;
		case 2:
			sprintf(dtc_char, "B%d%3X", is_sae_defined,number);
			break;
		case 3:
			sprintf(dtc_char, "U%d%3X", is_sae_defined,number);
			break;
		default:
			break;
	}
	return dtc_char;
}