#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <net/if.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <pthread.h>

#define CAN_INTERFACE "vcan0"
#define CHECK_ERRORS 10  /* 10 seconds*/
#define CHECK_WARNINGS 5 /*  5 seconds*/
#define MSG_LENGTH 8
#define SERVICE_1 1
#define SERVICE_2 2
#define SERVICE_3 3
#define SERVICE_7 7

/*
  The main code has to be a loop that is always reading from the CAN bus and the only thing it does is to filter all the information.
*/

int send_obd_message(int s, int data[], int length);
int receive_obd_message(int s, int service);

int setup_socket() {
  int s;
  struct sockaddr_can addr;
  struct ifreq ifr;

  // Create a socket
  if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
    perror("Socket creation error");
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

// void* call_service02(int socket_id) {
//   int data_s2[7] = {0x05, 0x02, 0x01, 0x00, 0x02, 0x00, 0x55};
//   send_obd_message(socket_id, data_s2, MSG_LENGTH);
//   receive_obd_message(socket_id);
// }

// void* call_service03(int socket_id) {
//   int data_s3[7] = {0x01, 0x03, 0x55, 0x55, 0x55, 0x55, 0x55};
//   send_obd_message(socket_id, data_s3, MSG_LENGTH);
//   receive_obd_message(socket_id);
// }

int main() {
  //pthread_t thread1, thread2;
  int socket_id = setup_socket();
  // receive_obd_message(socket_id); 
  while(true) {
    // [#bytes, mode, PID, A, B, C, D]
    int data_s1[7] = {0x02, 0x01, 0x01, 0x55, 0x55, 0x55, 0x55};
    send_obd_message(socket_id, data_s1, MSG_LENGTH);
    int mil_status = receive_obd_message(socket_id, SERVICE_1);

    if(mil_status == 2) {
      int data_s2[7] = {0x05, 0x02, 0x01, 0x00, 0x02, 0x00, 0x55};
      send_obd_message(socket_id, data_s2, MSG_LENGTH);
      receive_obd_message(socket_id, SERVICE_2);

      int data_s3[7] = {0x01, 0x03, 0x55, 0x55, 0x55, 0x55, 0x55};
      send_obd_message(socket_id, data_s3, MSG_LENGTH);
      receive_obd_message(socket_id, SERVICE_3);
    } else if (mil_status == 0) {
      int data_s7[7] = {0x01, 0x07, 0x55, 0x55, 0x55, 0x55, 0x55};
      send_obd_message(socket_id, data_s7, MSG_LENGTH);
      receive_obd_message(socket_id, SERVICE_7);
    } 
    sleep(1);
  }
  
  // Close socket
  printf("Halting program\n");
  if (close(socket_id) < 0) {
		perror("Error closing the Socket");
		return EXIT_FAILURE;
	}

  return 0;
}