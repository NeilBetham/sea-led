#include "light_controller.h"

#include <limits.h>

uint32_t lerp_color(uint32_t color1, uint32_t color2, uint32_t q15_percentage) {
  int32_t red1 = (color1 & 0xFF000000) >> 24;
  int32_t green1 = (color1 & 0x00FF0000) >> 16;
  int32_t blue1 = (color1 & 0x0000FF00) >> 8;
  int32_t alpha1 = (color1 & 0x000000FF);

  int32_t red2 = (color2 & 0xFF000000) >> 24;
  int32_t green2 = (color2 & 0x00FF0000) >> 16;
  int32_t blue2 = (color2 & 0x0000FF00) >> 8;
  int32_t alpha2 = (color2 & 0x000000FF);

  int32_t red_lerp = 0;
  int32_t green_lerp = 0;
  int32_t blue_lerp = 0;
  int32_t alpha_lerp = 0;

  red_lerp = ((((red2 - red1) * q15_percentage) >> 15) + red1);
  green_lerp = ((((green2 - green1) * q15_percentage) >> 15) + green1);
  blue_lerp = ((((blue2 - blue1) * q15_percentage) >> 15) + blue1);
  alpha_lerp = ((((alpha2 - alpha1) * q15_percentage) >> 15) + alpha1);

  uint32_t ret = (red_lerp << 24) | (green_lerp << 16) | (blue_lerp << 8) | (alpha_lerp);
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

uint32_t intensity_scaling(uint32_t color, uint8_t scaler) {
  uint32_t red = (color & 0xFF000000) >> 24;
  uint32_t green = (color & 0x00FF0000) >> 16;
  uint32_t blue = (color & 0x0000FF00) >> 8;
  uint32_t alpha = (color & 0x000000FF);

  uint32_t scaled_red = (red * scaler) >> 8;
  uint32_t scaled_green = (green * scaler) >> 8;
  uint32_t scaled_blue = (blue * scaler) >> 8;
  uint32_t scaled_alpha = (alpha * scaler) >> 8;

  return ((scaled_red & 0xFF) << 24) | ((scaled_green & 0xFF) << 16) | ((scaled_blue & 0xFF) << 8) | (scaled_alpha & 0xFF);
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
      *color = le_swizzle(intensity_scaling(_color1, _intensity));
      break;
    case ColorMode::fade:
      // Handle Fade
      *color = le_swizzle(intensity_scaling(calc_fade(time), _intensity));
      break;
    case ColorMode::jump:
      // Handle Jump
      *color = le_swizzle(intensity_scaling(calc_jump(time), _intensity));
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
  uint32_t lerp_phase_q15 = ((cycle_time % (CYCLE_TIME_QUARTER / _speed)) << 15) / (CYCLE_TIME_QUARTER / _speed);
  if(phase == 0) {
    return lerp_color(_color1, _color2, lerp_phase_q15);
  } else if(phase == 1) {
    return lerp_color(_color2, _color3, lerp_phase_q15);
  } else if(phase == 2) {
    return lerp_color(_color3, _color4, lerp_phase_q15);
  } else if(phase == 3) {
    return lerp_color(_color4, _color1, lerp_phase_q15);
  } else if(phase >= 4) {
    _cycle_start_dt = time;
    return lerp_color(_color1, _color2, lerp_phase_q15);
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
