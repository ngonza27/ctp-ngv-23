#include <stdio.h>
#include <stdlib.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <sys/socket.h>
#include <unistd.h>

#define CAN_ID_R 0x7E8
#define SERVICE_1 0x41
#define SERVICE_2 0x42
#define SERVICE_3 0x43
#define SERVICE_7 0x47
#define DTC_TO_PRINT 10

// int service_one(__u8 data[], int length);
//unsigned int *service_two(int s, __u8 data[]);
char (*service_three(int s, __u8 data[]))[6];
//unsigned int *service_seven(int s, __u8 data[]);

/**
 	@brief Receives the raw CAN frame information
	@param s socket where the data is received
	@return integer representing the need to further call other services
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
				// int mil_is_on = service_one(frame.data, length);
				// return mil_is_on;
				// break;
			case SERVICE_2: // Service 02
				// printf("Got into service02\n");
				// char (*detected_s2)[6] = service_three(s, frame.data);
				// for (int i=0; i < DTC_TO_PRINT; ++i){
				// 	printf("DTC LIST: %s\n", detected_s2[i]);
				// }
				break;
			case SERVICE_3: // Service 03
				printf("Got into service03\n");
				char (*detected_s3)[6] = service_three(s, frame.data);
				for (int i=0; i < DTC_TO_PRINT; ++i){
					printf("DTC LIST: %s\n", detected_s3[i]);
				}
				break;
			case SERVICE_7: // Service 07
			// printf("Got into service07\n");
			// 	char (*detected_s7)[6] = service_three(s, frame.data);
			// 	for (int i=0; i < DTC_TO_PRINT; ++i){
			// 		printf("DTC LIST: %s\n", detected_s7[i]);
			// 	}
				break;
			default:
				printf("Service not supported");
				return 2;
				break;
		}
	}	else {
		printf("Uninterested ID or no payload");
		return 2;
	}
  return 0;
}