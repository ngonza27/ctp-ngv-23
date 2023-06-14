#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>

int main() {
  // Create a socket
  int sockfd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
  if (sockfd < 0) {
    perror("socket");
    exit(1);
  }

  // Bind the socket to the vehicle speed CAN ID
  struct sockaddr_can addr;
  addr.can_family = AF_CAN;
  addr.can_addr.id = 0x7DF;
  if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("bind");
    exit(1);
  }

  // Send a request for the vehicle speed
  struct can_frame frame;
  frame.can_id = 0x7E8;
  frame.can_dlc = 8;
  memset(frame.data, 0, sizeof(frame.data));
  if (write(sockfd, &frame, sizeof(frame)) < 0) {
    perror("write");
    exit(1);
  }

  // Read the response from the vehicle
  int ret;
  while ((ret = read(sockfd, &frame, sizeof(frame))) > 0) {
    if (frame.can_id == 0x7E8) {
      // The response is the vehicle speed in kilometers per hour
      printf("Vehicle speed: %d km/h\n", frame.data[0]);
      break;
    }
  }

  close(sockfd);
  return 0;
}
