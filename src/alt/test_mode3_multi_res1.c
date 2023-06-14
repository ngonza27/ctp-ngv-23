#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>

#define CAN_INTERFACE "can0"  // Change this to match your CAN interface name

int main()
{
    int s;
    struct sockaddr_can addr;
    struct ifreq ifr;
    struct can_frame frame;
    int nbytes;
    char dtcResponse[4096] = "";  // Buffer to store DTC response
    
    // Create a socket
    if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
        perror("Socket error");
        return -1;
    }
    
    // Set the CAN interface name
    strcpy(ifr.ifr_name, CAN_INTERFACE);
    ioctl(s, SIOCGIFINDEX, &ifr);
    
    // Assign the CAN interface to the socket
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    
    if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Bind error");
        return -1;
    }

    if (setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, &frame, sizeof(frame)) < 0) {
      perror("setsockopt");
      return 1;
    }
    
    // Prepare the OBD-II diagnostic request
    frame.can_id = 0x7DF;
    frame.can_dlc = 8;
    frame.data[0] = 0x02;
    frame.data[1] = 0x03;
    frame.data[2] = 0x00;
    frame.data[3] = 0x00;
    frame.data[4] = 0x00;
    frame.data[5] = 0x00;
    frame.data[6] = 0x00;
    frame.data[7] = 0x00;
    
    // Send the diagnostic request
    if (write(s, &frame, sizeof(struct can_frame)) != sizeof(struct can_frame)) {
        perror("Write error");
        return -1;
    }
    
    // Read the response
    do {
        nbytes = read(s, &frame, sizeof(struct can_frame));
        
        if (nbytes >= 2 && frame.can_id == 0x7E8) {
            // Extract the DTC response from multiple CAN frames
            int i;
            for (i = 0; i < frame.can_dlc && frame.data[i] != 0x00; i++) {
                snprintf(dtcResponse + strlen(dtcResponse), sizeof(dtcResponse) - strlen(dtcResponse), "%02X ", frame.data[i]);
            }
        }
    } while (nbytes >= 0);
    
    // Print the DTC response
    printf("DTC Response: %s\n", dtcResponse);
    
    close(s);
    
    return 0;
}