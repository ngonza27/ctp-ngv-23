#include <stdio.h>
#include <stdlib.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <sys/socket.h>

#define CAN_ID_S 0x7DF

/**
 	@brief Sends the CAN frame information
	@param s socket where the data is received
  @param data makes up the frame information
  @param length determines the number of bytes 
	@return status of function execution
*/
int send_obd_message(int s, int *data, int length) {
  struct can_frame frame;

  frame.can_id = CAN_ID_S; // Set the frame ID
  frame.can_dlc = length; // Set the frame DLC
  for(int i = 0; i < length; ++i){ // Set the frame DATA
    frame.data[i] = data[i];
  }

  if (send(s, &frame, sizeof(struct can_frame), 0) != sizeof(struct can_frame)) {
    perror("Error sending OBD-II CAN message\n");
    exit(1);
  } else {
    return 0;
  }
}
