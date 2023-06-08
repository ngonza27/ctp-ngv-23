#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/can.h>
#include <linux/can/raw.h>


int main() {
  int s;
  struct sockaddr_can addr;
  struct ifreq ifr;
  struct can_frame frame;

  // Create a socket
  if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
    perror("socket");
    exit(1);
  }

  strcpy(ifr.ifr_name, "vcan0" );
	ioctl(s, SIOCGIFINDEX, &ifr);

  // Bind the socket to the vcan0 interface
  memset(&addr, 0, sizeof(addr));
  addr.can_family = AF_CAN;
  addr.can_ifindex = ifr.ifr_ifindex;
  if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    printf("Error en el bind!!");
    perror("bind");
    exit(1);
  }

  // Set the frame ID
  frame.can_id = 0x7DF;
  // Set the frame DLC
  frame.can_dlc = 8;
  frame.data[0] = 0x01;
  frame.data[1] = 0x0D;
  memset(frame.data + 2, 0xCC, 6);
  // Send the frame
  if (write(s, &frame, sizeof(struct can_frame)) != sizeof(struct can_frame)) {
    perror("write");
    exit(1);
  }

  // Close the socket
  if (close(s) < 0) {
		perror("Close");
		return 1;
	}

  return 0;
}