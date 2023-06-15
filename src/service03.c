#include <stdio.h>
#include <stdlib.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <sys/socket.h>

int service_three(int s) {
	struct can_frame frame;
	while (1) {
		if (receive_can_frame(s, &frame) == -1) {
			close(s);
			perror("Error receiving OBD-II CAN message\n");
			return EXIT_FAILURE;
		}
		
		// Check if the received frame contains diagnostic trouble codes
		if (frame.can_id == 0x7E8 && frame.can_dlc > 0) {
			int num_dtc = frame.data[1] >> 4;  // Number of DTCs reported in this frame
			
			// Process the DTCs reported in this frame
			for (int i = 0; i < num_dtc; i++) {
					unsigned int dtc = (frame.data[2 + (i * 2)] << 8) | frame.data[3 + (i * 2)];
					printf("DTC: %04X\n", dtc);
			}
		}
		
		// Check if there are more frames to receive
		if ((frame.data[0] & 0x10) != 0x10) {
			break;
		}
	}
  return 0;
}