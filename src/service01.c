#include <stdio.h>
#include <stdlib.h>
#include <asm-generic/int-ll64.h>

/**
 	@brief Extracts the MIL bit value (A7 bit)
	@param data represents the raw HEX data on a CAN frame
	@return value of the MIL bit
*/
int service_one(__u8 *data) {
  return (data[2] >> (sizeof(data[2]) * 8 - 1)) & 1;;
}