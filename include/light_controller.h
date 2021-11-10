/**
 * @brief Translates modes and colors into realtime color values
 */

#pragma once

#include "app_delegate.h"

class LightController : public AppDelegate {
public:
  void handle_command_update(
    uint32_t color1,
    uint32_t color2,
    uint32_t color3,
    uint32_t color4,
    uint8_t intensity,
    uint8_t speed,
    ColorMode mode
  );

  void get_colors(uint32_t time, uint32_t* color);


private:
  uint32_t _color1 = 0;
  uint32_t _color2 = 0;
  uint32_t _color3 = 0;
  uint32_t _color4 = 0;
  uint8_t _intensity = 0;
  uint8_t _speed = 0;
  ColorMode _mode = ColorMode::off;

  // State for color changing modes
  uint32_t _cycle_start_dt = 0;

  uint32_t calc_fade(uint32_t time);
  uint32_t calc_jump(uint32_t time);

};




