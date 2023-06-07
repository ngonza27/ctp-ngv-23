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
#define TIME_TO_WAIT 1 /*one second*/

int get_can_data(int s) {
  struct can_frame frame;

  frame.can_id = 0x7DF;
  frame.can_dlc = 8;
  frame.data[0] = 0x01;
  frame.data[1] = 0x0D;
  frame.data[2] = 0xCC;
  frame.data[3] = 0xCC;
  frame.data[4] = 0xCC;
  frame.data[5] = 0xCC;
  frame.data[6] = 0xCC;
  frame.data[7] = 0xCC;

  if (write(s, &frame, sizeof(frame)) < 0) {
    if (errno == EAGAIN) {
      printf("No data available.\n");
      return 0;
    } else {
      perror("Write");
      return 1;
    }
  }

  struct can_frame response;
  int nread = read(s, &response, sizeof(response));
  if (nread < 0) {
    if (errno == EAGAIN) {
      printf("No data available.\n");
      return 0;
    } else {
      perror("read");
      return 1;
    }
  }

  if (response.can_id != 0x7E8) {
    fprintf(stderr, "Unexpected response message from ID 0x%02X\n", response.can_id);
    return 1;
  }

  printf("Vehicle speed: %d km/h\n", response.data[0]);

  return 0;
}


int main() {
  int s;
  clock_t last = clock();
  while(1){
    if (clock() >= (last + TIME_TO_WAIT*CLOCKS_PER_SEC)){
      s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
      if (s < 0) {
        perror("Socket");
        return 1;
      }

      if (get_can_data(s) != 0) {
        close(s);
        return 1;
      }

      close(s);
      last = clock();
    }
  }
  return 0;
}
