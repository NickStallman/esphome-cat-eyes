#pragma once
#include <cstdint>
#include <cstring>

namespace cat_eyes {

// RGB565 conversion
inline uint16_t rgb565(uint8_t r, uint8_t g, uint8_t b) {
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

// Byte-swap for SPI (GC9A01 expects big-endian)
inline uint16_t rgb565_be(uint8_t r, uint8_t g, uint8_t b) {
  uint16_t c = rgb565(r, g, b);
  return (c >> 8) | (c << 8);
}

inline uint16_t swap16(uint16_t c) {
  return (c >> 8) | (c << 8);
}

// Preset palette definitions
static constexpr uint8_t PALETTE_AMBER[]  = {240, 190, 30};
static constexpr uint8_t PALETTE_GREEN[]  = {30, 220, 60};
static constexpr uint8_t PALETTE_BLUE[]   = {40, 120, 240};
static constexpr uint8_t PALETTE_RED[]    = {220, 40, 20};
static constexpr uint8_t PALETTE_PURPLE[] = {160, 50, 220};

static constexpr int GRADIENT_LUT_SIZE = 120;

struct ColorPalette {
  // Source colors (RGB888)
  uint8_t iris_center[3];
  uint8_t iris_mid[3];
  uint8_t iris_edge[3];
  uint8_t iris_ring[3];
  uint8_t fiber_color[3];
  uint8_t highlight[3];

  // Precomputed LUT (RGB565 big-endian, indexed by distance from center)
  uint16_t gradient_lut[GRADIENT_LUT_SIZE];

  // Precomputed RGB565 big-endian for common colors
  uint16_t bg_color;
  uint16_t ring_color;
  uint16_t fiber_color_565;
  uint16_t highlight_color;

  void set_from_rgb(uint8_t r, uint8_t g, uint8_t b) {
    iris_center[0] = r; iris_center[1] = g; iris_center[2] = b;

    // Derive other colors by darkening
    for (int i = 0; i < 3; i++) {
      iris_mid[i] = (uint8_t)(iris_center[i] * 0.70f);
      iris_edge[i] = (uint8_t)(iris_center[i] * 0.40f);
      iris_ring[i] = (uint8_t)(iris_center[i] * 0.25f);
      fiber_color[i] = (uint8_t)(iris_center[i] * 0.65f);
    }

    highlight[0] = 255; highlight[1] = 255; highlight[2] = 245;

    recompute_lut();
  }

  void set_preset(const char* name) {
    if (strcmp(name, "amber") == 0)       set_from_rgb(PALETTE_AMBER[0], PALETTE_AMBER[1], PALETTE_AMBER[2]);
    else if (strcmp(name, "green") == 0)  set_from_rgb(PALETTE_GREEN[0], PALETTE_GREEN[1], PALETTE_GREEN[2]);
    else if (strcmp(name, "blue") == 0)   set_from_rgb(PALETTE_BLUE[0], PALETTE_BLUE[1], PALETTE_BLUE[2]);
    else if (strcmp(name, "red") == 0)    set_from_rgb(PALETTE_RED[0], PALETTE_RED[1], PALETTE_RED[2]);
    else if (strcmp(name, "purple") == 0) set_from_rgb(PALETTE_PURPLE[0], PALETTE_PURPLE[1], PALETTE_PURPLE[2]);
  }

  void recompute_lut() {
    // Fill gradient: center → mid at 50%, mid → edge at 100%
    int half = GRADIENT_LUT_SIZE / 2;
    for (int i = 0; i < GRADIENT_LUT_SIZE; i++) {
      uint8_t r, g, b;
      if (i < half) {
        float t = (float)i / half;
        r = iris_center[0] + (int16_t)(iris_mid[0] - iris_center[0]) * t;
        g = iris_center[1] + (int16_t)(iris_mid[1] - iris_center[1]) * t;
        b = iris_center[2] + (int16_t)(iris_mid[2] - iris_center[2]) * t;
      } else {
        float t = (float)(i - half) / (GRADIENT_LUT_SIZE - half);
        r = iris_mid[0] + (int16_t)(iris_edge[0] - iris_mid[0]) * t;
        g = iris_mid[1] + (int16_t)(iris_edge[1] - iris_mid[1]) * t;
        b = iris_mid[2] + (int16_t)(iris_edge[2] - iris_mid[2]) * t;
      }
      gradient_lut[i] = rgb565_be(r, g, b);
    }

    bg_color = rgb565_be(0, 0, 0);
    ring_color = rgb565_be(iris_ring[0], iris_ring[1], iris_ring[2]);
    fiber_color_565 = rgb565_be(fiber_color[0], fiber_color[1], fiber_color[2]);
    highlight_color = rgb565_be(255, 255, 245);
  }
};

}  // namespace cat_eyes
