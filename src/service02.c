#include <stdio.h>
#include <stdlib.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <sys/socket.h>

int service_two(__u8 *data, int length) {

	for (int i = 0; i < length; i++) {
		printf("%02X ",data[i]);
  }
	printf("\r\n");

  return 0;
}