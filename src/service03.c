#include <stdio.h>
#include <stdlib.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <sys/socket.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

#define MAX_DTC_PER_FRAME 3
#define CAN_ID_R 0x7E8
#define SERVICE_2 0x42
#define SERVICE_3 0x43
#define SERVICE_7 0x47

char detected_DTC[3][6];

/**
 	@brief Decodes the hex value into readable DTC code
	@param dtc hex number of the reported code
	@return decoded DTC
*/
char *decode_dtc(unsigned int *dtc) {
	unsigned int category =  (*dtc >> 14) & 0x03;
	unsigned int is_sae_defined = (*dtc >> 12) & 0x03;
	unsigned int number = *dtc & 0xFFF;
	char* dtc_char = (char*)malloc(5 * sizeof(char));
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

/**
 	@brief Reads the DTCs from the CAN frame
	@param data represents the raw HEX data on a CAN frame
	@param counter keeps track of whre to put the processed DTC
	@param keep_reading should keep waiting for CAN messages?
	@param total_dtc number of DTC on the current CAN frame
	@return void
*/
void extract_DTC(__u8 *data, int *counter, bool *keep_reading, int *total_dtc) {
	int size = (*total_dtc < MAX_DTC_PER_FRAME) ? *total_dtc : MAX_DTC_PER_FRAME;
	for (int i = 0; i < size; ++i) {
		unsigned int dtc = (data[2+(i*2)]<<8) | data[3+(i*2)];
		char* decoded_dtc = decode_dtc(&dtc);
		strncpy(detected_DTC[(*counter)++], decoded_dtc, sizeof(detected_DTC[i])-1);
		free(decoded_dtc);
	}
	(*keep_reading) = ((*total_dtc)-MAX_DTC_PER_FRAME)>0;
	(*total_dtc) -= MAX_DTC_PER_FRAME;
}

/**
 	@brief Processes the CAN response for Service03
	@param s socket where the data is received
	@param data represents the raw HEX data on a CAN frame
	@return lsit of decoded DTCs
*/
char (*service_three(int s, __u8 *data))[6] {
	int counter = 0;
	int total_dtc = data[0];
	bool keep_reading = false;
	
	extract_DTC(data, &counter, &keep_reading, &total_dtc);

	struct can_frame frame;
	while (keep_reading) {
		if (recv(s, &frame, sizeof(struct can_frame), 0) == -1) {
			close(s);
			perror("Error receiving OBD-II CAN message\n");
			return (char (*)[6])EXIT_FAILURE;
		}
		if (frame.can_id == CAN_ID_R && frame.can_dlc > 0) {
			switch (frame.data[1]) {
				case SERVICE_2:
				case SERVICE_3:
				case SERVICE_7:
					total_dtc = frame.data[0];  // Number of DTCs reported in this frame
					extract_DTC(frame.data, &counter, &keep_reading, &total_dtc);
					break;
				default:
					printf("Service not supported");
					return (char (*)[6])EXIT_FAILURE;
					break; 
			}
		} else {
			printf("Uninterested ID or no payload");
			return (char (*)[6])EXIT_FAILURE;
		}
	}
  return detected_DTC;
}