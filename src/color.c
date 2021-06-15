#include "color.h"

#include <stdint.h>
#include <math.h>

void hsl_to_rgb(float h, float s, float l, float* r, float* g, float* b) {
  float chroma = l * s;
  float hue_prime = fmod(h / (60.0f), 6);
  float x = chroma * (1.0f - fabs(fmod(hue_prime, 2) - 1.0f));
  float m = l - chroma;
  *r = 0.0f;
  *g = 0.0f;
  *b = 0.0f;

  switch((uint8_t)hue_prime) {
    case 0:  *r = chroma; *g = x;      *b = 0; break;
    case 1:  *r = x;      *g = chroma; *b = 0; break;
    case 2:  *r = 0;      *g = chroma; *b = x; break;
    case 3:  *r = 0;      *g = x;      *b = chroma; break;
    case 4:  *r = x;      *g = 0;      *b = chroma; break;
    case 5:  *r = chroma; *g = 0;      *b = x; break;
    default: *r = 0;      *g = 0;      *b = 0; break;
  }

  *r += m;
  *g += m;
  *b += m;
}

