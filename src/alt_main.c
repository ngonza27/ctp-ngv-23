#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/can.h>
#include <linux/can/raw.h>

int main() {
  // Open the CAN interface.
  int fd = open("/dev/can0", O_RDWR | O_NONBLOCK);
  if (fd < 0) {
    perror("open");
    return 1;
  }

  // Set the bitrate.
  struct can_config cfg = {
    .bitrate = 500000,
  };
  if (ioctl(fd, CAN_SET_BITRATE, &cfg) < 0) {
    perror("ioctl");
    return 1;
  }

  // Send the OBD PID request.
  struct can_frame frame = {
    .can_id = 0x7DF,
    .can_dlc = 8,
    .data = {0x01, 0x0D, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC},
  };
  if (write(fd, &frame, sizeof(frame)) < 0) {
    if (errno == EAGAIN) {
      printf("No data available.\n");
      return 0;
    } else {
      perror("write");
      return 1;
    }
  }

  // Receive the response message.
  struct can_frame response;
  int nread = read(fd, &response, sizeof(response));
  if (nread < 0) {
    if (errno == EAGAIN) {
      printf("No data available.\n");
      return 0;
    } else {
      perror("read");
      return 1;
    }
  }

  // Check if the response message is from the expected sender.
  if (response.can_id != 0x7E8) {
    fprintf(stderr, "Unexpected response message from ID 0x%02X\n", response.can_id);
    return 1;
  }

  // Print the vehicle speed.
  printf("Vehicle speed: %d km/h\n", response.data[0]);

  // Close the CAN interface.
  close(fd);

  return 0;
}
