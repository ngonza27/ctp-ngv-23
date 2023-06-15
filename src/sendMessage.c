#include <stdio.h>
#include <stdlib.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <sys/socket.h>

int send_obd_message(int s, int *data, int length) {
  struct can_frame frame;

  frame.can_id = 0x7DF; // Set the frame ID
  frame.can_dlc = length; // Set the frame DLC
  for(int i = 0; i < length; ++i){ // Set the frame DATA
    frame.data[i] = data[i];
  }

  if (send(s, &frame, sizeof(struct can_frame), 0) != sizeof(struct can_frame)) {
    perror("Error sending OBD-II CAN message\n");
    exit(1);
  } else {
    printf("Message sent\n");
    return 0;
  }
}