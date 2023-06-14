#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/can.h>
#include <linux/can/raw.h>

#define CAN_INTERFACE "can0"
#define CAN_ID 0x7DF
#define IAT_RESPONSE_ID 0x7E8
#define IAT_REQUEST_PID 0x01
#define IAT_RESPONSE_PID 0x41

int main() {
    int s;
    struct sockaddr_can addr;
    struct ifreq ifr;
    struct can_frame frame;
    struct can_filter rfilter[1];

    // Create socket
    s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (s < 0) {
        perror("Socket creation error");
        return -1;
    }

    // Set CAN interface name
    strcpy(ifr.ifr_name, CAN_INTERFACE);
    ioctl(s, SIOCGIFINDEX, &ifr);

    // Bind the socket to the CAN interface
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Socket bind error");
        return -1;
    }

    // Set filter for receiving IAT response
    rfilter[0].can_id = IAT_RESPONSE_ID;
    rfilter[0].can_mask = CAN_SFF_MASK;
    setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter));

    // Prepare IAT request frame
    frame.can_id = CAN_ID;
    frame.can_dlc = 8;
    frame.data[0] = 0x02;
    frame.data[1] = IAT_REQUEST_PID;
    memset(frame.data + 2, 0xFF, 6); // Set remaining bytes to 0xFF

    // Send IAT request frame
    if (write(s, &frame, sizeof(frame)) != sizeof(frame)) {
        perror("Socket write error");
        return -1;
    }

    // Receive and process IAT response
    while (1) {
        if (read(s, &frame, sizeof(frame)) > 0) {
            if (frame.can_id == IAT_RESPONSE_ID && frame.data[1] == IAT_RESPONSE_PID) {
                // Extract IAT value from response
                int iat = frame.data[3] - 40; // Offset of -40 degrees Celsius

                printf("Intake Air Temperature: %d degrees Celsius\n", iat);
                break;
            }
        }
    }

    // Close the socket
    close(s);

    return 0;
}
