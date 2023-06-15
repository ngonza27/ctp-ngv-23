#include <stdio.h>
#include <stdlib.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <sys/socket.h>
#include <unistd.h>


// int service_one(__u8 data[], int length);
// int service_two(__u8 data[], int length);
unsigned int * service_three(int s);
//int service_seven(__u8 data[], int length);

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

	unsigned int *detected_DTC = service_three(s);
	for (int i=0; i < 10; ++i){
		printf("DTC LIST: %02X\n", detected_DTC[i]);
	}

	// switch (service) {
	// 	case 65: // Service 01
	// 		int mil_is_on = service_one(frame.data, length);
	// 		if (mil_is_on) {
	// 			perror("Vehicle Error Detected\n");
	// 			return 1;
	// 		}
	// 		break;
	// 	case 66: // Service 02
	// 		break;
	// 	case 67: // Service 03
	// 		service_three(frame.data, length);
	// 		break;
	// 	case 71: // Service 07
	// 		service_seven(frame.data, length);
	// 		break;
	// }

	printf("\r\n");
  return 0;
}