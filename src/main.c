#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <linux/can.h>
#include <linux/can/raw.h>

int
main() {
  int s;
  struct ifreq ifr;
  struct sockaddr_can addr;
  struct can_frame frame;

  
  if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
    perror("Socket");
    return 1;
  }
  strcpy(ifr.ifr_name, "vcan0" );
  ioctl(s, SIOCGIFINDEX, &ifr);

  addr.can_family = AF_CAN;
  addr.can_ifindex = ifr.ifr_ifindex;

  if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("Bind");
    return 1;
  }

  struct can_frame {
    canid_t can_id;  /* 32 bit CAN_ID + EFF/RTR/ERR flags */
    __u8    can_dlc; /* frame payload length in byte (0 .. 8) */
    __u8    __pad;   /* padding */
    __u8    __res0;  /* reserved / padding */
    __u8    __res1;  /* reserved / padding */
    __u8    data[8] __attribute__((aligned(8)));
  };
  
  frame.can_id = 0x555; /* ID */
  frame.can_dlc = 5;    /* # f bytes */
  sprintf(frame.data, "01 01");

  if (write(s, &frame, sizeof(struct can_frame)) != sizeof(struct can_frame)) {
    perror("Write");
    return 1;
  }

  nbytes = read(s, &frame, sizeof(struct can_frame));

  if (nbytes < 0) {
    perror("can raw socket read");
    return 1;
  }
}