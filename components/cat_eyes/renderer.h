#pragma once
#include <cstdint>
#include "eye_state.h"
#include "palette.h"
#include "gc9a01.h"

namespace cat_eyes {

static constexpr int EYE_SIZE = 240;
static constexpr int EYE_RADIUS = EYE_SIZE / 2;
static constexpr int LID_RANGE = (int)(EYE_SIZE * 0.55f);  // 132

class EyeRenderer {
 public:
  void init(ColorPalette* palette);

  // Render one full frame to a display via scanline push
  void render_frame(GC9A01Driver* display, const EyeState& state, bool mirror);

  // Advance internal shimmer timer (call once per frame)
  void tick();

 private:
  ColorPalette* palette_;
  float fiber_angles_[28];       // Float angles for trig
  float t_ = 0.0f;              // Internal timer for shimmer

  // Scanline buffer
  uint16_t line_buf_[EYE_SIZE];

  // Render single scanline into line_buf_
  void render_scanline(int y, const EyeState& state, bool mirror);

  // Lid boundary: returns the Y threshold at pixel column x
  // Pixels above this Y are covered by the top lid (drawn as black)
  float lid_top_at(int x, float lid_top, float lid_top_bend, float openness, bool mirror);
  float lid_bot_at(int x, float lid_bottom, float lid_bottom_bend, float openness, bool mirror);

  // Pupil geometry
  bool is_inside_pupil(int px_x, int px_y, int pupil_cx, int pupil_cy,
                       float pw, float ph, int iris_r);

  // Iris color at given distance from center (uses gradient LUT)
  uint16_t iris_color_at(int dist_sq, int iris_r);

  // Check if pixel is on a fiber line
  bool is_on_fiber(int x, int y, int cx, int cy, int iris_r, bool mirror);

  // Highlight blending (returns modified color or original)
  uint16_t apply_highlight(uint16_t base_color, int px_x, int px_y,
                           int hl_x, int hl_y, int hl_r,
                           int hl2_x, int hl2_y, int hl2_r);
};

}  // namespace cat_eyes
