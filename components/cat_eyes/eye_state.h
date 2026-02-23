#pragma once
#include <cstdint>
#include <cmath>

namespace cat_eyes {

// Field mask bits for merge_state overlay
enum EyeField : uint16_t {
  F_PUPIL_WIDTH    = 1 << 0,
  F_PUPIL_HEIGHT   = 1 << 1,
  F_GAZE_X         = 1 << 2,
  F_GAZE_Y         = 1 << 3,
  F_IRIS_SIZE      = 1 << 4,
  F_LID_TOP        = 1 << 5,
  F_LID_BOTTOM     = 1 << 6,
  F_LID_TOP_BEND   = 1 << 7,
  F_LID_BOTTOM_BEND = 1 << 8,
  F_HIGHLIGHT_SIZE = 1 << 9,
  F_HIGHLIGHT_X    = 1 << 10,
  F_HIGHLIGHT_Y    = 1 << 11,
  F_EYE_OPENNESS   = 1 << 12,
  F_ALL            = 0x1FFF,
};

static constexpr int EYE_STATE_FIELD_COUNT = 13;

struct EyeState {
  float pupil_width    = 0.35f;
  float pupil_height   = 0.85f;
  float gaze_x         = 0.0f;
  float gaze_y         = 0.0f;
  float iris_size      = 0.92f;
  float lid_top        = 0.26f;
  float lid_bottom     = 0.18f;
  float lid_top_bend   = 0.45f;
  float lid_bottom_bend = 0.30f;
  float highlight_size = 0.18f;
  float highlight_x    = -0.25f;
  float highlight_y    = -0.30f;
  float eye_openness   = 0.85f;

  // Access fields by index (for lerp/merge loops)
  float& field(int i) { return (&pupil_width)[i]; }
  float field(int i) const { return (&pupil_width)[i]; }
};

// Default relaxed cat eye state
static constexpr EyeState DEFAULT_STATE = {
  0.35f,   // pupil_width
  0.85f,   // pupil_height
  0.0f,    // gaze_x
  0.0f,    // gaze_y
  0.92f,   // iris_size
  0.26f,   // lid_top
  0.18f,   // lid_bottom
  0.45f,   // lid_top_bend
  0.30f,   // lid_bottom_bend
  0.18f,   // highlight_size
  -0.25f,  // highlight_x
  -0.30f,  // highlight_y
  0.85f,   // eye_openness
};

// Linearly interpolate between two fully-specified states
inline EyeState lerp_state(const EyeState& a, const EyeState& b, float t) {
  EyeState result;
  for (int i = 0; i < EYE_STATE_FIELD_COUNT; i++) {
    result.field(i) = a.field(i) + (b.field(i) - a.field(i)) * t;
  }
  return result;
}

// Merge overlay fields into base. mask bits indicate which overlay fields are active.
inline EyeState merge_state(const EyeState& base, const EyeState& overlay, uint16_t mask) {
  EyeState result = base;
  for (int i = 0; i < EYE_STATE_FIELD_COUNT; i++) {
    if (mask & (1 << i)) {
      result.field(i) = overlay.field(i);
    }
  }
  return result;
}

// Keyframe state: partial state (only some fields set) + which fields are present
struct KeyframeState {
  EyeState state;
  uint16_t mask;  // which fields are specified (non-None in Python)
};

// Resolve a keyframe against a base state: fill in unset fields from base
inline EyeState resolve_keyframe(const EyeState& base, const KeyframeState& kf) {
  return merge_state(base, kf.state, kf.mask);
}

// Lerp between two resolved (fully-specified) states
// If a field was unset in both keyframes, it inherited from base, so it's fine to lerp

}  // namespace cat_eyes
