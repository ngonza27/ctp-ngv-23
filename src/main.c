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

#define CAN_INTERFACE "vcan0"

/*
  El codigo prinicpal tiene que ser un ciclo que siempre este leyendo del bus CAN y lo unico que hace es filtrar toda la informacion.
*/

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

  // Bind the socket to the vcan0 interface
  strcpy(ifr.ifr_name, CAN_INTERFACE);
  ioctl(s, SIOCGIFINDEX, &ifr);

  memset(&addr, 0, sizeof(addr));
  addr.can_family = AF_CAN;
  addr.can_ifindex = ifr.ifr_ifindex;
  if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("Bind error");
    return EXIT_FAILURE;
  }

  // rfilter[0].can_id = IAT_RESPONSE_ID;
  // rfilter[0].can_mask = CAN_SFF_MASK;
  // setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter));
  return s;
}

int main() {
  int socket_id = setup_socket();
  //int socket_id1 = setup_socket();
  int length = 7; // [#bytes, mode, PID, A, B, C, D]
  int data[7] = {0x01, 0x0D, 0xFF, 0x0D, 0x0D, 0x0D, 0x0D};
  
  //send_obd_message(socket_id, data, length);
  receive_obd_message(socket_id);

  // Close the socket
  if (close(socket_id) < 0) {
		perror("Error closing the Socket");
		return EXIT_FAILURE;
	}

  return 0;
}