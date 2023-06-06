#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/can.h>
#include <linux/can/raw.h>

int main() {
  int s;
  struct ifreq ifr;
  structy sockaddr_can addr;
  struct can_frame frame;


  if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
    perror("Socket");
    return 1; 
  }

  strcpy(ifr.ifr_name, "vcan0");
  ioctl(s, SIOCGIFINDEX, &ifr);

  addr.can_family = AF_CAN;
  addr.can_ifindex = ifr.ifr_ifindex;

  if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("Bind");
    return 1;
  }

  frame.can_id = 0x7DF;
  frame.can_dlc = 8;
  frame.data[0] = 0x02;
  frame.data[1] = 0x01;
  frame.data[2] = 0x0C;
  frame.data[3] = 0xCC;
  frame.data[4] = 0xCC;
  frame.data[5] = 0xCC;
  frame.data[6] = 0xCC;
  frame.data[7] = 0xCC;

  if (write(s, &frame, sizeof(frame)) < 0) {
    perror("Write");
    return 1;
  }

  struct can_frame response;
  int nread = read(s, &response, sizeof(response));
  if (nread < 0) {
    perror("read");
    return 1;
  }

  if (response.can_id != 0x7E8) {
    fprintf(stderr, "Unexpected response message from ID 0x%02X\n", response.can_id);
    return 1;
  }

  printf("Vehicle speed: %d km/h\n", response.data[0]);

  close(s);

  return 0;
}
