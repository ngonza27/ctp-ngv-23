#include <stdio.h>
#include <stdlib.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <sys/socket.h>
#include <stdbool.h>
#include <unistd.h>

#define MAX_DTC_PER_FRAME 3
unsigned int detected_DTC[10];

void extract_DTC(__u8 *data, int *counter, bool *keep_reading, int *total_dtc) {
	int size = (*total_dtc < MAX_DTC_PER_FRAME) ? *total_dtc : MAX_DTC_PER_FRAME;
	for (int i = 0; i < size; ++i) {
		unsigned int dtc = (data[2+(i*2)]<<8) | data[3+(i*2)];
		detected_DTC[(*counter)++] = dtc;
		// printf("DTC: %04X\n", dtc);
	}
	(*keep_reading) = ((*total_dtc)-MAX_DTC_PER_FRAME)>0;
	(*total_dtc) -= MAX_DTC_PER_FRAME;
}

unsigned int *service_three(int s, __u8 *data, int length) {
	// printf("CAN DLC: %02X\n", length);
	// printf("RESPONSE SID: %02X\n", data[0]);
	int counter = 0;
	int total_dtc = data[1];
	bool keep_reading = false;
	
	extract_DTC(data, &counter, &keep_reading, &total_dtc);

	struct can_frame frame;
	while (keep_reading) {
		if (recv(s, &frame, sizeof(struct can_frame), 0) == -1) {
			close(s);
			perror("Error receiving OBD-II CAN message\n");
			return (unsigned int *)EXIT_FAILURE;
		}
		// printf("CAN ID: %02X\n", frame.can_id);
		// printf("CAN DLC: %02X\n", frame.can_dlc);
		// printf("RESPONSE SID: %02X\n", frame.data[0]);
		if (frame.can_id == 0x7E8 && frame.can_dlc > 0) {
			switch (frame.data[0]) {
				case 0x43:
				case 0x42:
				case 0x47:
					total_dtc = frame.data[1];  // Number of DTCs reported in this frame
					// printf("TOTAL DTC: %d\n", total_dtc);
					extract_DTC(frame.data, &counter, &keep_reading, &total_dtc);
					break;
				default:
					break;
			}
		}
	}
  return detected_DTC;
}