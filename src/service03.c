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

char detected_DTC[10][6];

char *decode_dtc(unsigned int *dtc) {
	unsigned int category =  (*dtc >> 14) & 0x03;
	unsigned int is_sae_defined = (*dtc >> 12) & 0x03;
	unsigned int number = *dtc & 0xFFF;
	char* dtc_char = (char*)malloc(5 * sizeof(char));
	switch (category) {
		case 0:
			sprintf(dtc_char, "P%d%3X ", is_sae_defined,number);
			break;
		case 1:
			sprintf(dtc_char, "C%d%3X ", is_sae_defined,number);
			break;
		case 2:
			sprintf(dtc_char, "B%d%3X ", is_sae_defined,number);
			break;
		case 3:
			sprintf(dtc_char, "U%d%3X ", is_sae_defined,number);
			break;
		default:
			break;
	}
	return dtc_char;
}

void extract_DTC(__u8 *data, int *counter, bool *keep_reading, int *total_dtc) {
	int size = (*total_dtc < MAX_DTC_PER_FRAME) ? *total_dtc : MAX_DTC_PER_FRAME;
	for (int i = 0; i < size; ++i) {
		unsigned int dtc = (data[2+(i*2)]<<8) | data[3+(i*2)];
		char* decoded_dtc = decode_dtc(&dtc);
		strncpy(detected_DTC[(*counter)++], decoded_dtc, sizeof(detected_DTC[i]) - 1);
		free(decoded_dtc);
	}
	(*keep_reading) = ((*total_dtc)-MAX_DTC_PER_FRAME)>0;
	(*total_dtc) -= MAX_DTC_PER_FRAME;
}

char (*service_three(int s, __u8 *data, int length))[6] {
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
		// printf("CAN ID: %02X\n", frame.can_id);
		// printf("CAN DLC: %02X\n", frame.can_dlc);
		// printf("RESPONSE SID: %02X\n", frame.data[1]);
		if (frame.can_id == CAN_ID_R && frame.can_dlc > 0) {
			switch (frame.data[1]) {
				case SERVICE_2:
				case SERVICE_3:
				case SERVICE_7:
					total_dtc = frame.data[0];  // Number of DTCs reported in this frame
					// printf("TOTAL DTC: %d\n", total_dtc);
					extract_DTC(frame.data, &counter, &keep_reading, &total_dtc);
					break;
				default:
					printf("Service not supported");
					break; 
			}
		}
	}
  return detected_DTC;
}