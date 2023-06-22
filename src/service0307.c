#include <stdio.h>
#include <stdlib.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <sys/socket.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

#define MAX_DTC_PER_FRAME 3
#define CAN_ID_R 0x7E8
#define SERVICE_3 0x43
#define SERVICE_7 0x47


char detected_DTC[3][6];
char nullArray[][6] = {{'\0', '\0', '\0', '\0', '\0', '\0'}};
char* decode_dtc(unsigned int *data);

/**
 	@brief Reads the DTCs from the CAN frame
	@param data represents the raw HEX data on a CAN frame
	@param counter keeps track of where to put the processed DTC
	@param keep_reading should keep waiting for CAN messages?
	@param total_dtc number of DTC on the current CAN frame
	@return void
*/
void extract_DTC(__u8 *data, int *counter, bool *keep_reading, int *total_dtc) {
	int size = (*total_dtc < MAX_DTC_PER_FRAME) ? *total_dtc : MAX_DTC_PER_FRAME;
	for (int i = 0; i < size; ++i) {
		unsigned int dtc = (data[2+(i*2)]<<8) | data[3+(i*2)];
		char* decoded_dtc = decode_dtc(&dtc);
		strncpy(detected_DTC[(*counter)++], decoded_dtc, sizeof(detected_DTC[i])-1);
		free(decoded_dtc);
	}
	(*keep_reading) = ((*total_dtc)-MAX_DTC_PER_FRAME)>0;
	(*total_dtc) -= MAX_DTC_PER_FRAME;
}

/**
 	@brief Processes the CAN response for Service03
	@param s socket where the data is received
	@param data represents the raw HEX data on a CAN frame
	@return list of decoded DTCs
*/
char (*service_three_seven(int s, __u8 *data))[6] {
	int counter = 0;
	int total_dtc = data[0];
	bool keep_reading = false;
	
	extract_DTC(data, &counter, &keep_reading, &total_dtc);

	struct can_frame frame;
	while (keep_reading) {
		if (recv(s, &frame, sizeof(struct can_frame), 0) == -1) {
			close(s);
			perror("Error receiving OBD-II CAN message\n");
			return nullArray;
		}
		printf("%d %d\n",frame.data[2], frame.data[1]);
		if (frame.can_id == CAN_ID_R && frame.can_dlc > 0) {
			switch (frame.data[1]) {
				case SERVICE_3:
				case SERVICE_7:
					total_dtc = frame.data[0];  // Number of DTCs reported in this frame
					extract_DTC(frame.data, &counter, &keep_reading, &total_dtc);
					break;
				default:
					if (total_dtc == 0){
						printf("No more data(%d)\n", frame.data[1]);
					return nullArray;
					} else {
						continue;
					}
			}
		} else {
			printf("Uninterested ID or no payload {S3, S7}");
			continue;
			// return nullArray;
		}
	}
  return detected_DTC;
}
