#include <stdio.h>
#include <stdlib.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <sys/socket.h>

int receive_obd_message(int s) {
  int nbytes;
  struct can_frame frame;
  
	nbytes = recv(s, &frame, sizeof(struct can_frame), 0);

 	if (nbytes < 0) {
    perror("Error receiving OBD-II CAN message\n");
		return 1;
	}
  printf("0x%03X [%d] ",frame.can_id, frame.can_dlc);

	for (int i = 0; i < frame.can_dlc; i++) {
		printf("%02X ",frame.data[i]);
  }

	printf("\r\n");
  return 0;
}