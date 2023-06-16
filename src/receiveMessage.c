#include <stdio.h>
#include <stdlib.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <sys/socket.h>
#include <unistd.h>


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

	if (frame.can_id == 0x7E8 && length > 0) {
		printf("entro al switch case con servicio %d", service);
		switch (service) {
			case 0x41: // Service 01
				// int mil_is_on = service_one(frame.data, length);
				// if (mil_is_on) {
				// 	perror("Vehicle Error Detected\n");
				// 	return 1;
				// }
				break;
			case 0x42: // Service 02
				printf("Entro al service02");
				unsigned int *detected_s2 = service_three(s, frame.data, length);
				for (int i=0; i < 10; ++i){
					printf("DTC LIST: %02X\n", detected_s2[i]);
				}
				break;
			case 0x43: // Service 03
				unsigned int *detected_s3 = service_three(s, frame.data, length);
				for (int i=0; i < 10; ++i){
					printf("DTC LIST: %02X\n", detected_s3[i]);
				}
				break;
			case 0x47: // Service 07
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