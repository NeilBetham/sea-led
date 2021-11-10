/**
 * @brief Handles updates produced by the app
 */

#pragma once


#include <stdint.h>


// The board has no audio input or accel so no music or wave modes
enum class ColorMode {
  off = 0,
  single,
  fade,
  jump,
  music,
  wave,
};


class AppDelegate {
public:
  virtual ~AppDelegate() {};

  virtual void handle_command_update(
    uint32_t color1,
    uint32_t color2,
    uint32_t color3,
    uint32_t color4,
    uint8_t intensity,
    uint8_t speed,
    ColorMode mode
  ) = 0;
};
