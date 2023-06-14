#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <net/if.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/can.h>
#include <linux/can/raw.h>

int send_obd_message(int s, int data[], int length);
int receive_obd_message(int s);

int setup_socket() {
  int s;
  struct sockaddr_can addr;
  struct ifreq ifr;

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

  return s;
}

int main() {
  int socket_id = setup_socket();
  //int socket_id1 = setup_socket();
  int length = 8;
  int data[8] = {0x01, 0x0D, 0xFF, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D};
  
  send_obd_message(socket_id, data, length);
  
  // Close the socket
  if (close(socket_id) < 0) {
		perror("Close");
		return 1;
	}

  return 0;
}