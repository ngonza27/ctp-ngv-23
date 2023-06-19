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
//unsigned int *service_two(int s, __u8 data[], int length);
char (*service_three(int s, __u8 data[], int length))[6];
//unsigned int *service_seven(int s, __u8 data[], int length);

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
				// if (mil_is_on) {
				// 	perror("Vehicle Error Detected\n");
				// 	return 1;
				// }
				break;
			case SERVICE_2: // Service 02
				// printf("Entro al service02\n");
				// char *detected_s2 = (*service_three(s, frame.data, length))[5];
				// for (int i=0; i < 10; ++i){
				// 	printf("DTC LIST: %s\n", detected_s2[i]);
				// }
				break;
			case SERVICE_3: // Service 03
			printf("Entro al service03\n");
				char (*detected_s3)[6] = service_three(s, frame.data, length);
				for (int i=0; i < DTC_TO_PRINT; ++i){
					printf("DTC LIST: %s\n", detected_s3[i]);
				}
				break;
			case SERVICE_7: // Service 07
			// printf("Entro al service07\n");
			// 	char *detected_s7 = (*service_three(s, frame.data, length))[5];
			// 	for (int i=0; i < 10; ++i){
			// 		printf("DTC LIST: %s\n", detected_s7[i]);
			// 	}
				break;
			default:
				break;
		}
	}	else {
		printf("Uninterested ID or no payload");
	}

	printf("\r\n");
  return 0;
}