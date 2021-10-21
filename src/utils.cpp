#include "utils.h"

#include "timing.h"

void sleep(uint32_t milliseconds) {
  uint32_t start_count = sys_now();
  uint32_t end_count = start_count + milliseconds;
  uint32_t current_count = start_count;

  while(current_count >= start_count && current_count < end_count) {
    for(uint32_t index = 0; index < 1000; index++);
    current_count = sys_now();
  }
}
