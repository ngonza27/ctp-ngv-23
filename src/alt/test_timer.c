#include <stdio.h>
#include <time.h>
#include <unistd.h>

#define TIME_TO_WAIT 2 /* one second */

int get_can_data() {
  printf("Vehicle speed: 50 km/h\n");
  return 0;
}

int main() {
  time_t last = time(NULL);
  
  while (1) {
    time_t current = time(NULL);
    if (current >= last + TIME_TO_WAIT) {
      if (get_can_data() != 0) {
        return 1;
      }
      last = current;
    }
  }
  
  return 0;
}
