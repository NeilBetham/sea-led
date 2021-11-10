#include "light_controller.h"

#include <limits.h>

uint32_t lerp_color(uint32_t color1, uint32_t color2, uint32_t q15_percentage) {
  uint32_t red1 = (color1 & 0xFF000000) >> 8;
  uint32_t green1 = (color1 & 0x00FF0000);
  uint32_t blue1 = (color1 & 0x0000FF00) << 8;
  uint32_t alpha1 = (color1 & 0x000000FF) << 16;

  uint32_t red2 = (color2 & 0xFF000000) >> 8;
  uint32_t green2 = (color2 & 0x00FF0000);
  uint32_t blue2 = (color2 & 0x0000FF00) << 8;
  uint32_t alpha2 = (color2 & 0x000000FF) << 16;

  uint32_t red_lerp = ((((red2 - red1) / q15_percentage) << 16) + red1);
  uint32_t green_lerp = ((((green2 - green1) / q15_percentage) << 16) + green1);
  uint32_t blue_lerp = ((((blue2 - blue1) / q15_percentage) << 16) + blue1);
  uint32_t alpha_lerp = ((((alpha2 - alpha1) / q15_percentage) << 16) + alpha1);

  uint32_t ret = (red_lerp << 8) | (green_lerp) | (blue_lerp >> 8) | (alpha_lerp >> 16);
  return ret;
}

uint32_t le_swizzle(uint32_t value) {
  uint32_t ret = 0;
  ret = ((value & 0xFF000000) >> 24) |
        ((value & 0x00FF0000) >> 8) |
        ((value & 0x0000FF00) << 8) |
        ((value & 0x000000FF) << 24);
  return ret;
}


// Longest cycle time
constexpr uint32_t MAX_CYCLE_TIME_MS = 255000;
constexpr uint32_t CYCLE_TIME_MULT = 1000;
constexpr uint32_t CYCLE_TIME_QUARTER = MAX_CYCLE_TIME_MS / 4;

void LightController::handle_command_update(
    uint32_t color1,
    uint32_t color2,
    uint32_t color3,
    uint32_t color4,
    uint8_t intensity,
    uint8_t speed,
    ColorMode mode) {
  _color1 = color1;
  _color2 = color2;
  _color3 = color3;
  _color4 = color4;
  _intensity = intensity;
  _speed = speed;
  _mode = mode;
}

void LightController::get_colors(uint32_t time, uint32_t* color) {
  // Break out color handling by mode
  // This board only supports 4 out of the six
  switch(_mode) {
    case ColorMode::off:
      *color = 0;
      break;
    case ColorMode::single:
      *color = le_swizzle(_color1);
      break;
    case ColorMode::fade:
      // Handle Fade
      *color = le_swizzle(calc_fade(time));
      break;
    case ColorMode::jump:
      // Handle Jump
      *color = le_swizzle(calc_jump(time));
      break;
    default:
      *color = 0;
  }
}

uint32_t LightController::calc_fade(uint32_t time) {
  // First get the time spent in the current cycle
  uint32_t cycle_time = 0;
  if(time < _cycle_start_dt) {
    // Handle the rollover
    cycle_time = UINT32_MAX - _cycle_start_dt + time;
  } else {
    cycle_time = time - _cycle_start_dt;
  }

  // Second get the phase we are in so we know which colors to blend
  uint32_t phase = cycle_time / (CYCLE_TIME_QUARTER / _speed);
  uint32_t lerp_phase_q16 = ((cycle_time % (CYCLE_TIME_QUARTER / _speed)) << 16) / (CYCLE_TIME_QUARTER / _speed);
  if(phase == 0) {
    return lerp_color(_color1, _color2, lerp_phase_q16);
  } else if(phase == 1) {
    return lerp_color(_color2, _color3, lerp_phase_q16);
  } else if(phase == 2) {
    return lerp_color(_color3, _color4, lerp_phase_q16);
  } else if(phase == 3) {
    return lerp_color(_color4, _color1, lerp_phase_q16);
  } else if(phase >= 4) {
    _cycle_start_dt = time;
    return lerp_color(_color1, _color2, lerp_phase_q16);
  }

  return 0;
}

uint32_t LightController::calc_jump(uint32_t time) {
  // First get the time spent in the current cycle
  uint32_t cycle_time = 0;
  if(time < _cycle_start_dt) {
    // Handle the rollover
    cycle_time = UINT32_MAX - _cycle_start_dt + time;
  } else {
    cycle_time = time - _cycle_start_dt;
  }

  // Divide the cycle time by four and get the color
  uint32_t phase = cycle_time / (CYCLE_TIME_QUARTER / _speed);
  if(phase == 0) {
    return _color1;
  } else if(phase == 1) {
    return _color2;
  } else if(phase == 2) {
    return _color3;
  } else if(phase == 3) {
    return _color4;
  } else if(phase >= 4) {
    _cycle_start_dt = time;
    return _color1;
  }

  return 0;
}
