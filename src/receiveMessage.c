#include <stdio.h>
#include <stdlib.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>

#define CAN_ID_R 0x7E8
#define SERVICE_1 0x41
#define SERVICE_2 0x42
#define SERVICE_3 0x43
#define SERVICE_7 0x47
#define DTC_TO_PRINT 10

int service_one(__u8 data[]);
char (*service_two(__u8 data[]));
char (*service_three_seven(int s, __u8 data[]))[6];

/**
 	@brief Receives the raw CAN frame information
	@param s socket where the data is received
	@return function execution status
*/
int receive_obd_message(int s) {
  int nbytes;
  struct can_frame frame;
  
	nbytes = recv(s, &frame, sizeof(struct can_frame), 0);

 	if (nbytes < 0) {
		close(s);
    perror("Error receiving OBD-II CAN message\n");
		return EXIT_FAILURE;
	}

	int service = frame.data[1];
	int length = frame.can_dlc;

	if (frame.can_id == CAN_ID_R && length > 0) {
		switch (service) {
			case SERVICE_1: // Service 01
				printf("Got into service01\n");
				int mil_is_on = service_one(frame.data);
				char* status = "OFF";
				status = mil_is_on ? "ON" : status;
				printf("DTC That caused the freeze frame: %s\n", status);
				return 2;
				break;
			case SERVICE_2: // Service 02
				printf("Got into service02\n");
				char* dtc = service_two(frame.data);
				printf("DTC That caused the freeze frame: %s\n", dtc);
				break;
			case SERVICE_3: // Service 03
			case SERVICE_7: // Service 07
				printf("Got into service03/07\n");
				char (*detected_s3)[6] = service_three_seven(s, frame.data);
				for (int i=0; i < DTC_TO_PRINT; ++i) {
					if((detected_s3[i] != NULL) && (*detected_s3[i] != '\0'))	 {
						printf("DTC LIST: %s\n", detected_s3[i]);
					}
				}
				break;
			default:
				printf("Service not supported%2X\n", service);
				return EXIT_FAILURE;
				break;
		}
	}	else {
		printf("Uninterested ID or no payload\n");
		return EXIT_FAILURE;
	}
  return 0;
}