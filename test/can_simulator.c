#include <stdio.h>
#include <stdint.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <net/if.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#define CAN_INTERFACE "vcan0"
#define CAN_ID_S 0x7DF
#define CAN_ID_R 0x7E8
#define SERVICE_1 0x41
#define SERVICE_2 0x42
#define SERVICE_3 0x43
#define SERVICE_7 0x47
#define SERVICE_1_R 0x01
#define SERVICE_2_R 0x02
#define SERVICE_3_R 0x03
#define SERVICE_7_R 0x07

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

int send_obd_message(int s, int service) {
	int length_list[] = {4, 6, 8};
	int dtc_length_opt[] = {0x00, 0x01, 0x02, 0x03};
	int info[] = {0xd8, 0x81, 0xda, 0x4b, 0xa4, 0x40, 0x1a, 0x31, 0x61, 0x5c, 0xc6, 0xf8, 0x51, 0x9a, 0x61, 0x92, 0x43, 0xb4, 0x1a, 0xc0, 0xab, 0xa5, 0xf8, 0x24, 0x55, 0x5d, 0x29, 0x12, 0xae, 0x96, 0x95, 0x9b, 0x58, 0x76, 0x9d, 0x78, 0xf4, 0x17, 0x37, 0xa2, 0xc8, 0xff, 0xc8, 0xb9, 0xd4, 0xd7, 0xb0, 0xa1, 0xb1, 0x64, 0xf0, 0xb4, 0xa4, 0xd9, 0xcd, 0x38, 0xf5, 0xa8, 0x65, 0x32, 0xe1, 0x7c, 0xae, 0x72, 0x88, 0x77, 0x35, 0x5b, 0x15, 0x09, 0x78, 0x09, 0x39, 0x2a, 0xf6, 0x79, 0x22, 0x7a, 0x60, 0x07, 0x1f, 0x80, 0x8d, 0x29, 0xe8, 0xe5, 0xef, 0x4c, 0x25, 0x2b, 0xdd, 0xb9, 0x1d, 0x0e, 0x13};
	int id_list[] = {/*0x24, 0x25, 0xaa, 0xb4, 0x1c4, 0x1d0,0x1d2, 0x1d3, 0x223,  0x224, 0x260, 0x2c1, 0x320, 0x343, 0x344, 0x380, 0x381, 0x389, 0x38f, 0x399, 0x3a5, 0x3b0, 0x3b1, 0x3b7, 0x3bc, 0x3e8, 0x3e9, 0x3f9, 0x411, 0x412, 0x413, 0x414, 0x420, 0x45a, 0x489, 0x48a, 0x48b, 0x4ac, 0x4cb, 0x4d3, 0x4ff, 0x610, 0x611, 0x614, 0x615, 0x619, 0x61a, 0x620, 0x621, 0x622, 0x623, 0x624, 0x638, 0x63c, 0x63d, 0x640, 0x680, 0x6f3, 0x770, 0x778, 0x7c6, 0x7ce, 0x7e0, 0x7e1, 0x7e2, 0x7e3, 0x7e4, 0x7e5, 0x7e6, 0x7e7,*/ 0x7e8};
	char data[8] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};

	srand(time(NULL));

	int id_index = rand() % (sizeof(id_list) / sizeof(id_list[0]));
	int length_index = rand() % (sizeof(length_list) / sizeof(length_list[0]));
	int dtc_idx = rand() % (sizeof(dtc_length_opt) / sizeof(dtc_length_opt[0]));

	int length = length_list[length_index];
	struct can_frame frame;
  frame.can_id = id_list[id_index]; // Set the frame ID
  frame.can_dlc = length; // Set the frame DLC

	// Fill data array with random values from info[]
	for (int i = 0; i < length; i++) {
		int info_index = rand() % (sizeof(info) / sizeof(info[0]));
		frame.data[i] = info[info_index];		
	}
	
	switch (service) {
		case 1: // Service 01
			frame.data[1] = SERVICE_1;
			break;
		case 2: // Service 02
			frame.data[1] = SERVICE_2;
			break;
		case 3: // Service 03
		frame.data[0] = dtc_length_opt[dtc_idx];
			frame.data[1] = SERVICE_3;
			break;
		case 7: // Service 07
			frame.data[0] = dtc_length_opt[dtc_idx];
			frame.data[1] = SERVICE_7;
			break;
		default:
			break;
	}

	printf("Sending [%2X,%2X,%2X,%2X,%2X,%2X,%2X,%2X]\n",frame.data[0],frame.data[1],frame.data[2],frame.data[3],frame.data[4],frame.data[5],frame.data[6],frame.data[7]);
  if (send(s, &frame, sizeof(struct can_frame),0) != sizeof(struct can_frame)) {
    perror("Error sending OBD-II CAN message\n");
    exit(1);
  }

	return 0;
}

int receive_obd_message(int s) {
  int nbytes;
  struct can_frame frame;
  
 	if (recv(s, &frame, sizeof(struct can_frame), 0) < 0) {
		close(s);
    perror("Error receiving OBD-II CAN message\n");
		return 1;
	}

	int service = frame.data[1];
	int length = frame.can_dlc;
	printf("Received service%2X\n", service);
	if (frame.can_id == CAN_ID_S && length > 0) {
		switch (service) {
			case SERVICE_1_R: // Service 01
				printf("Respond service1\n");
				send_obd_message(s, 1);
				break;
			case SERVICE_2_R: // Service 02
				printf("Respond service2\n");
				send_obd_message(s, 2);
				break;
			case SERVICE_3_R: // Service 03
				printf("Respond service3\n");
				send_obd_message(s, 3);
				break;
			case SERVICE_7_R: // Service 07
				printf("Respond service4\n");
				send_obd_message(s, 7);
				break;
			default:
				return 1;
		}
	}	else {
		return 1;
	}
  return 0;
}

int main(int argc, char **argv) {
  int s = setup_socket();
	while (1) {
		send_obd_message(s, 0);
	  receive_obd_message(s);
		sleep(1);
	}

	return 0;
}
