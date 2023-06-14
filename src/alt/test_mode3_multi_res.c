#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>

#define CAN_INTERFACE "can0"  // Replace with your CAN interface name

int main() {
    int s;  // Socket descriptor
    struct sockaddr_can addr;
    struct ifreq ifr;

    // Create a socket
    if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
        perror("Socket creation failed");
        return 1;
    }

    // Specify the CAN interface
    strcpy(ifr.ifr_name, CAN_INTERFACE);
    ioctl(s, SIOCGIFINDEX, &ifr);

    // Set the CAN interface options
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    // Bind the socket to the CAN interface
    if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Socket bind failed");
        close(s);
        return 1;
    }

    // Prepare the request message
    struct can_frame req_frame;
    req_frame.can_id = 0x7DF;  // Diagnostic request ID
    req_frame.can_dlc = 8;
    req_frame.data[0] = 0x02;  // Service 3 request (Diagnostic Trouble Codes)
    req_frame.data[1] = 0x01;  // Current powertrain diagnostic data
    req_frame.data[2] = 0x00;  // Request parameter 1
    req_frame.data[3] = 0x00;  // Request parameter 2
    req_frame.data[4] = 0x00;  // Request parameter 3
    req_frame.data[5] = 0x00;  // Request parameter 4
    req_frame.data[6] = 0x00;  // Request parameter 5
    req_frame.data[7] = 0x00;  // Request parameter 6

    // Send the request message
    if (write(s, &req_frame, sizeof(struct can_frame)) != sizeof(struct can_frame)) {
        perror("Request message sending failed");
        close(s);
        return 1;
    }

    // Receive and process the response messages
    struct can_frame resp_frame;
    while (1) {
        ssize_t nbytes = read(s, &resp_frame, sizeof(struct can_frame));
        if (nbytes < 0) {
            perror("Read error");
            close(s);
            return 1;
        }

        // Check if the response message is relevant to Service 3
        if (resp_frame.can_id == 0x7E8 && resp_frame.data[0] == 0x03) {
            // Extract the DTCs from the response message
            int numDTCs = resp_frame.data[1] / 2;  // Each DTC is 2 bytes
            for (int i = 0; i < numDTCs; i++) {
                uint16_t dtc = (resp_frame.data[2 + (i * 2)] << 8) | resp_frame.data[3 + (i * 2)];
                printf("DTC: %04X\n", dtc);
            }

            // Check if there are more response messages to follow
            if (!(resp_frame.data[0] & 0x80)) {
                break;  // No more response messages
            }
        }
    }

    close(s);
    return 0;
}
