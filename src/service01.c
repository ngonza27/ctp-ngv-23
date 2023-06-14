#include <stdio.h>
#include <stdlib.h>
#include <asm-generic/int-ll64.h>

int service_one(__u8 *data, int length) {
  return data[3] & 1;
}