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

// int service_one(__u8 data[], int length);
//unsigned int * service_two(int s, __u8 data[], int length);
unsigned int * service_three(int s, __u8 data[], int length);
//unsigned int * service_seven(int s, __u8 data[], int length);

int receive_obd_message(int s) {
  int nbytes;
  struct can_frame frame;
  
	nbytes = recv(s, &frame, sizeof(struct can_frame), 0);

 	if (nbytes < 0) {
		close(s);
    perror("Error receiving OBD-II CAN message\n");
		return EXIT_FAILURE;
	}

	int service = frame.data[0];
	int length = frame.can_dlc;

	if (frame.can_id == CAN_ID_R && length > 0) {
		printf("entro al switch case con servicio %d", service);
		switch (service) {
			case SERVICE_1: // Service 01
				// int mil_is_on = service_one(frame.data, length);
				// if (mil_is_on) {
				// 	perror("Vehicle Error Detected\n");
				// 	return 1;
				// }
				break;
			case SERVICE_2: // Service 02
				printf("Entro al service02");
				unsigned int *detected_s2 = service_three(s, frame.data, length);
				for (int i=0; i < 10; ++i){
					printf("DTC LIST: %02X\n", detected_s2[i]);
				}
				break;
			case SERVICE_3: // Service 03
				unsigned int *detected_s3 = service_three(s, frame.data, length);
				for (int i=0; i < 10; ++i){
					printf("DTC LIST: %02X\n", detected_s3[i]);
				}
				break;
			case SERVICE_7: // Service 07
				unsigned int *detected_s7 = service_three(s, frame.data, length);
				for (int i=0; i < 10; ++i){
					printf("DTC LIST: %02X\n", detected_s7[i]);
				}
				break;
			default:
				printf("Service not supported");
				break;
		}
	}	else {
		printf("Uninterested ID or no payload");
	}

	printf("\r\n");
  return 0;
}