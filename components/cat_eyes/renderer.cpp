#include "renderer.h"
#include <cmath>
#include <cstdlib>

namespace cat_eyes {

// Alpha blend two RGB565 big-endian colors. alpha is 0-255.
static uint16_t blend_color(uint16_t base_be, uint16_t overlay_be, uint8_t alpha) {
  // Convert from big-endian to native
  uint16_t base = swap16(base_be);
  uint16_t over = swap16(overlay_be);

  // Extract RGB components
  uint8_t br = (base >> 11) & 0x1F;
  uint8_t bg = (base >> 5) & 0x3F;
  uint8_t bb = base & 0x1F;

  uint8_t or_ = (over >> 11) & 0x1F;
  uint8_t og = (over >> 5) & 0x3F;
  uint8_t ob = over & 0x1F;

  // Blend
  uint8_t inv = 255 - alpha;
  uint8_t rr = (br * inv + or_ * alpha) / 255;
  uint8_t rg = (bg * inv + og * alpha) / 255;
  uint8_t rb = (bb * inv + ob * alpha) / 255;

  uint16_t result = (rr << 11) | (rg << 5) | rb;
  return swap16(result);
}

void EyeRenderer::init(ColorPalette* palette) {
  palette_ = palette;
  t_ = 0.0f;

  // Initialize 28 fiber angles with slight random offsets (matching Python)
  for (int i = 0; i < 28; i++) {
    float base_angle = i * (2.0f * M_PI / 28.0f);
    float offset = ((rand() % 80) - 40) / 1000.0f;  // ±0.04
    fiber_angles_[i] = base_angle + offset;
  }
}

void EyeRenderer::tick() {
  t_ += 1.0f / 60.0f;
}

void EyeRenderer::render_frame(GC9A01Driver* display, const EyeState& state, bool mirror) {
  display->begin_scanline_write();
  for (int y = 0; y < EYE_SIZE; y++) {
    render_scanline(y, state, mirror);
    display->push_scanline(line_buf_, EYE_SIZE);
  }
  display->end_scanline_write();
}

void EyeRenderer::render_scanline(int y, const EyeState& state, bool mirror) {
  const int cx = EYE_RADIUS;
  const int cy = EYE_RADIUS;

  // Read state values
  float iris_size = state.iris_size;
  float gaze_x = state.gaze_x;
  float gaze_y = state.gaze_y;
  float pw = state.pupil_width;
  float ph = state.pupil_height;
  float lid_top = state.lid_top;
  float lid_bottom = state.lid_bottom;
  float lid_top_bend = state.lid_top_bend;
  float lid_bottom_bend = state.lid_bottom_bend;
  float hl_size = state.highlight_size;
  float hl_x_norm = state.highlight_x;
  float hl_y_norm = state.highlight_y;
  float openness = state.eye_openness;

  if (mirror) hl_x_norm = -hl_x_norm;

  int iris_r = (int)(EYE_RADIUS * iris_size);
  int iris_r_sq = iris_r * iris_r;

  // Pupil center offset by gaze
  int gaze_range = (int)(iris_r * 0.3f);
  int pupil_cx = cx + (int)(gaze_x * gaze_range);
  int pupil_cy = cy + (int)(gaze_y * gaze_range);

  // Highlight positions
  float hl_gx = hl_x_norm + gaze_x * 0.1f;
  float hl_gy = hl_y_norm + gaze_y * 0.1f;
  int hl_px = cx + (int)(hl_gx * iris_r);
  int hl_py = cy + (int)(hl_gy * iris_r);
  int hl_r = (int)(iris_r * hl_size);
  if (hl_r < 3) hl_r = 3;

  // Secondary highlight
  float sec_offset_x = mirror ? -0.3f : 0.3f;
  int hl2_px = cx + (int)((hl_gx * 0.4f + sec_offset_x) * iris_r);
  int hl2_py = cy + (int)((hl_gy * 0.4f + 0.2f) * iris_r);
  int hl2_r = hl_r / 3;
  if (hl2_r < 2) hl2_r = 2;

  // Outer iris ring: ring_w pixels wide, minimum 2px
  int ring_w = (int)(iris_r * 0.07f);
  if (ring_w < 2) ring_w = 2;
  int ring_inner = iris_r - ring_w;
  int ring_inner_sq = ring_inner * ring_inner;

  // Pre-compute Y distance from center
  int dy_from_center = y - cy;
  int dy_sq = dy_from_center * dy_from_center;

  for (int x = 0; x < EYE_SIZE; x++) {
    // Check if pixel is within the round display area
    int dx = x - cx;
    if (dx * dx + dy_sq > EYE_RADIUS * EYE_RADIUS) {
      line_buf_[x] = palette_->bg_color;
      continue;
    }

    // Check lid coverage
    float top_y = lid_top_at(x, lid_top, lid_top_bend, openness, mirror);
    if ((float)y < top_y) {
      line_buf_[x] = palette_->bg_color;
      continue;
    }

    float bot_y = lid_bot_at(x, lid_bottom, lid_bottom_bend, openness, mirror);
    if ((float)y > bot_y) {
      line_buf_[x] = palette_->bg_color;
      continue;
    }

    // Distance from iris center (always display center)
    int iris_dx = x - cx;
    int iris_dy = y - cy;
    int dist_sq = iris_dx * iris_dx + iris_dy * iris_dy;

    // Outside iris? Black background
    if (dist_sq > iris_r_sq) {
      line_buf_[x] = palette_->bg_color;
      continue;
    }

    // Check pupil
    if (is_inside_pupil(x, y, pupil_cx, pupil_cy, pw, ph, iris_r)) {
      // Apply highlight even inside pupil
      line_buf_[x] = apply_highlight(palette_->bg_color, x, y,
                                     hl_px, hl_py, hl_r, hl2_px, hl2_py, hl2_r);
      continue;
    }

    // Iris gradient color
    uint16_t color;

    // Outer ring
    if (dist_sq > ring_inner_sq) {
      color = palette_->ring_color;
    }
    // Fiber check
    else if (is_on_fiber(x, y, cx, cy, iris_r, mirror)) {
      color = palette_->fiber_color_565;
    }
    // Iris gradient
    else {
      color = iris_color_at(dist_sq, iris_r);
    }

    // Apply highlight on top of iris
    color = apply_highlight(color, x, y, hl_px, hl_py, hl_r, hl2_px, hl2_py, hl2_r);

    line_buf_[x] = color;
  }
}

float EyeRenderer::lid_top_at(int x, float lid_top, float lid_top_bend,
                               float openness, bool mirror) {
  // Port of Python's _draw_lids top lid curve
  // Returns the Y coordinate of the bottom edge of the top lid at column x
  const int cx = EYE_RADIUS;

  float squeeze = (1.0f - openness) * EYE_RADIUS * 0.4f;
  float almond_top = EYE_RADIUS * 1.05f * fmaxf(0.15f, 1.0f - lid_top * 0.5f);

  // Right-eye convention
  float x_eval = mirror ? (float)(EYE_SIZE - x) : (float)x;
  float t = (x_eval - cx) / (float)EYE_RADIUS;
  float at = fabsf(t);

  // Inner/outer power asymmetry
  float power = (t < 0) ? 1.8f : 4.5f;
  float taper = almond_top * powf(at, power);

  float bend_scale = lid_top_bend * EYE_RADIUS * 0.25f;
  float bend = bend_scale * (-t);

  float center_y = lid_top * LID_RANGE + squeeze;
  float y_val = center_y + taper + bend;

  return fmaxf(0.0f, y_val);
}

float EyeRenderer::lid_bot_at(int x, float lid_bottom, float lid_bottom_bend,
                               float openness, bool mirror) {
  const int cx = EYE_RADIUS;

  float squeeze = (1.0f - openness) * EYE_RADIUS * 0.4f;
  float almond_bot = EYE_RADIUS * 1.05f * fmaxf(0.15f, 1.0f - lid_bottom * 0.5f);

  float x_eval = mirror ? (float)(EYE_SIZE - x) : (float)x;
  float t = (x_eval - cx) / (float)EYE_RADIUS;
  float at = fabsf(t);

  // Bottom lid: flatter curve
  float power = (t < 0) ? 2.0f : 3.5f;
  float taper = almond_bot * powf(at, power);

  float bend_scale = lid_bottom_bend * EYE_RADIUS * 0.20f;
  float bend = bend_scale * (-t);

  float center_y_b = lid_bottom * LID_RANGE + squeeze;
  float y_val = EYE_SIZE - (center_y_b + taper) - bend;

  return fminf((float)EYE_SIZE, y_val);
}

bool EyeRenderer::is_inside_pupil(int px_x, int px_y, int pupil_cx, int pupil_cy,
                                   float pw, float ph, int iris_r) {
  int pupil_v = (int)(iris_r * ph * 0.75f);
  int pupil_h = (int)(iris_r * pw * 0.6f);
  if (pupil_h < 2) pupil_h = 2;

  if (pw > 0.65f) {
    // Near-round: ellipse
    float blend = fminf((pw - 0.65f) / 0.35f, 1.0f);
    int h = (int)(pupil_h * (1.0f - blend) + pupil_v * blend);
    // Ellipse test: ((x-cx)/h)² + ((y-cy)/v)² <= 1
    float ex = (float)(px_x - pupil_cx) / (float)h;
    float ey = (float)(px_y - pupil_cy) / (float)pupil_v;
    return (ex * ex + ey * ey) <= 1.0f;
  }

  // Cat slit — two arc test
  if (pupil_h < 2) pupil_h = 2;
  float arc_r = ((float)pupil_v * pupil_v + (float)pupil_h * pupil_h) / (2.0f * pupil_h);
  float arc_cx_left = pupil_cx - (arc_r - pupil_h);
  float arc_cx_right = pupil_cx + (arc_r - pupil_h);

  // Point is inside the slit if it's inside the left arc AND inside the right arc
  float dl_x = px_x - arc_cx_left;
  float dl_y = px_y - pupil_cy;
  float dist_left_sq = dl_x * dl_x + dl_y * dl_y;

  float dr_x = px_x - arc_cx_right;
  float dr_y = px_y - pupil_cy;
  float dist_right_sq = dr_x * dr_x + dr_y * dr_y;

  // Also check vertical bounds
  int dy = abs(px_y - pupil_cy);
  if (dy > pupil_v) return false;

  // Inside left arc (center is to the left, so inside = dist <= arc_r AND x >= arc_cx_left)
  // Inside right arc (center is to the right, so inside = dist <= arc_r AND x <= arc_cx_right)
  return (dist_left_sq <= arc_r * arc_r) && (dist_right_sq <= arc_r * arc_r);
}

uint16_t EyeRenderer::iris_color_at(int dist_sq, int iris_r) {
  // Map distance to LUT index
  float dist = sqrtf((float)dist_sq);
  float frac = dist / (float)iris_r;
  int idx = (int)(frac * (GRADIENT_LUT_SIZE - 1));
  if (idx < 0) idx = 0;
  if (idx >= GRADIENT_LUT_SIZE) idx = GRADIENT_LUT_SIZE - 1;
  return palette_->gradient_lut[idx];
}

bool EyeRenderer::is_on_fiber(int x, int y, int cx, int cy, int iris_r, bool mirror) {
  int dx = x - cx;
  int dy = y - cy;
  int dist_sq = dx * dx + dy * dy;

  int inner_r = (int)(iris_r * 0.18f);
  int outer_r = (int)(iris_r * 0.82f);
  int inner_sq = inner_r * inner_r;
  int outer_sq = outer_r * outer_r;

  // Must be within fiber ring
  if (dist_sq < inner_sq || dist_sq > outer_sq) return false;

  // Check angle against each fiber
  float px_angle = atan2f((float)dy, (float)dx);

  for (int j = 0; j < 28; j++) {
    float shimmer = sinf(t_ * 1.5f + j * 0.7f) * 0.02f;
    float a = fiber_angles_[j] + shimmer;
    if (mirror) a = M_PI - a;

    // Normalize angle difference to [-π, π]
    float diff = px_angle - a;
    while (diff > M_PI) diff -= 2.0f * M_PI;
    while (diff < -M_PI) diff += 2.0f * M_PI;

    // Fiber width: ~1 pixel at the given distance
    float dist = sqrtf((float)dist_sq);
    float angular_width = 1.0f / dist;  // ~1 pixel wide

    if (fabsf(diff) < angular_width) return true;
  }
  return false;
}

uint16_t EyeRenderer::apply_highlight(uint16_t base_color, int px_x, int px_y,
                                       int hl_x, int hl_y, int hl_r,
                                       int hl2_x, int hl2_y, int hl2_r) {
  // Primary highlight
  int dx1 = px_x - hl_x;
  int dy1 = px_y - hl_y;
  int dist1_sq = dx1 * dx1 + dy1 * dy1;
  int hl_r_sq = hl_r * hl_r;

  if (dist1_sq < hl_r_sq) {
    float dist1 = sqrtf((float)dist1_sq);
    float frac = dist1 / (float)hl_r;

    // Inner bright core (r/3)
    int core_r = hl_r / 3;
    if (core_r < 1) core_r = 1;
    if (dist1_sq < core_r * core_r) {
      // Bright white highlight core — alpha ~210/255
      return blend_color(base_color, palette_->highlight_color, 210);
    }

    // Gradient falloff: alpha = 160 * (1 - frac)^0.6
    float alpha_f = 160.0f * powf(1.0f - frac, 0.6f);
    uint8_t alpha = (uint8_t)fminf(255.0f, alpha_f);
    if (alpha > 5) {
      return blend_color(base_color, palette_->highlight_color, alpha);
    }
  }

  // Secondary highlight
  int dx2 = px_x - hl2_x;
  int dy2 = px_y - hl2_y;
  int dist2_sq = dx2 * dx2 + dy2 * dy2;
  int hl2_r_sq = hl2_r * hl2_r;

  if (dist2_sq < hl2_r_sq) {
    float dist2 = sqrtf((float)dist2_sq);
    int core2_r = hl2_r / 2;
    if (core2_r < 1) core2_r = 1;
    if (dist2_sq < core2_r * core2_r) {
      return blend_color(base_color, palette_->highlight_color, 180);
    }
    // Outer ring of secondary
    return blend_color(base_color, palette_->highlight_color, 120);
  }

  return base_color;
}

}  // namespace cat_eyes
