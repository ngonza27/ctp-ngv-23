#include <stdio.h>
#include <stdlib.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <sys/socket.h>
#include <stdbool.h>
#include <unistd.h>

#define MAX_DTC_PER_FRAME 3
unsigned int detected_DTC[10];

unsigned int *service_three(int s) {
	struct can_frame frame;
	bool keep_reading = true;
	int total_dtc;
	int counter = 0;
	while (keep_reading) {
		if (recv(s, &frame, sizeof(struct can_frame), 0) == -1) {
			close(s);
			perror("Error receiving OBD-II CAN message\n");
			return (unsigned int *)EXIT_FAILURE;
		}
		printf("CAN ID: %02X\n", frame.can_id);
		printf("CAN DLC: %02X\n", frame.can_dlc);
		printf("RESPONSE SID: %02X\n", frame.data[0]);

		if (frame.can_id == 0x7E8 && frame.can_dlc > 0 && frame.data[0] == 0x43) {
			total_dtc = frame.data[1];  // Number of DTCs reported in this frame
			printf("TOTAL DTC: %d\n", total_dtc);
			int size = (total_dtc < MAX_DTC_PER_FRAME) ? total_dtc : MAX_DTC_PER_FRAME;
			for (int i = 0; i < size; ++i) {
					unsigned int dtc = (frame.data[2+(i*2)]<<8) | frame.data[3+(i*2)];
					detected_DTC[counter] = dtc;
					counter++;
					//TODO: DTC Trranslation is missing
					printf("DTC: %04X\n", dtc);
			}
			keep_reading = (total_dtc-MAX_DTC_PER_FRAME > 0) ? true : false;
			total_dtc = total_dtc - MAX_DTC_PER_FRAME;
			printf("Remaining DTC to read:%d\n",total_dtc);
		}
	}
  return detected_DTC;
}