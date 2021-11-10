/**
 * @brief Highly specific DMX driver
 */

#pragma once

#include <stdint.h>

class DMX {
public:
  void init(uint32_t refresh_time_ms);

  // This assumes channel 1 is red, 2 is green and 3 is blue
  uint32_t& color() { return *((uint32_t*)&_buffer[1]); };

  void tick();

private:
  uint8_t _buffer[11] = {0};
  uint32_t _refresh_time_ms = 0;
  uint32_t _last_refresh_ms = 0;

  void tx();
};
