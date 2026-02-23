#pragma once
#include "animation.h"

// Macro helpers for building keyframes.
// KF(time, easing, mask, field_values...)
// The mask uses F_* bits from eye_state.h to indicate which fields are set.
// Fields in the EyeState are positional — unset fields keep their default value
// but are ignored during resolve because mask says they're absent.

namespace cat_eyes {

// Helper: build a KeyframeState with given mask and state
#define KFS(m, ...) { EyeState __VA_ARGS__, (uint16_t)(m) }

// ============================================================
// blink — 0.30s, 6 keyframes
// ============================================================
static const Keyframe kf_blink[] = {
  {0.0f,  {EyeState{.pupil_width=0.35f}, F_PUPIL_WIDTH}, EasingType::LINEAR},
  {0.06f, {EyeState{.pupil_width=0.30f, .lid_top=0.65f, .lid_bottom=0.30f}, F_PUPIL_WIDTH|F_LID_TOP|F_LID_BOTTOM}, EasingType::EASE_IN},
  {0.10f, {EyeState{.pupil_width=0.28f, .lid_top=1.0f, .lid_bottom=0.75f}, F_PUPIL_WIDTH|F_LID_TOP|F_LID_BOTTOM}, EasingType::EASE_IN},
  {0.14f, {EyeState{.pupil_width=0.28f, .lid_top=1.0f, .lid_bottom=0.75f}, F_PUPIL_WIDTH|F_LID_TOP|F_LID_BOTTOM}, EasingType::LINEAR},
  {0.20f, {EyeState{.pupil_width=0.33f, .lid_top=0.40f, .lid_bottom=0.22f}, F_PUPIL_WIDTH|F_LID_TOP|F_LID_BOTTOM}, EasingType::EASE_OUT},
  {0.30f, {EyeState{}, 0}, EasingType::EASE_OUT},
};

// ============================================================
// slow_blink — 1.6s, 6 keyframes
// ============================================================
static const Keyframe kf_slow_blink[] = {
  {0.0f,  {EyeState{}, 0}, EasingType::LINEAR},
  {0.35f, {EyeState{.pupil_width=0.40f, .gaze_y=0.05f, .lid_top=0.70f, .lid_bottom=0.40f}, F_PUPIL_WIDTH|F_GAZE_Y|F_LID_TOP|F_LID_BOTTOM}, EasingType::EASE_IN_OUT},
  {0.55f, {EyeState{.pupil_width=0.42f, .gaze_y=0.08f, .lid_top=1.0f, .lid_bottom=0.75f}, F_PUPIL_WIDTH|F_GAZE_Y|F_LID_TOP|F_LID_BOTTOM}, EasingType::EASE_IN},
  {1.0f,  {EyeState{.pupil_width=0.42f, .gaze_y=0.08f, .lid_top=1.0f, .lid_bottom=0.75f}, F_PUPIL_WIDTH|F_GAZE_Y|F_LID_TOP|F_LID_BOTTOM}, EasingType::LINEAR},
  {1.3f,  {EyeState{.pupil_width=0.38f, .gaze_y=0.03f, .lid_top=0.35f, .lid_bottom=0.22f}, F_PUPIL_WIDTH|F_GAZE_Y|F_LID_TOP|F_LID_BOTTOM}, EasingType::EASE_OUT},
  {1.6f,  {EyeState{}, 0}, EasingType::EASE_OUT},
};

// ============================================================
// double_blink — 0.55s, 10 keyframes
// ============================================================
static const Keyframe kf_double_blink[] = {
  {0.0f,  {EyeState{}, 0}, EasingType::LINEAR},
  {0.05f, {EyeState{.lid_top=0.80f, .lid_bottom=0.45f}, F_LID_TOP|F_LID_BOTTOM}, EasingType::EASE_IN},
  {0.09f, {EyeState{.lid_top=1.0f, .lid_bottom=0.75f}, F_LID_TOP|F_LID_BOTTOM}, EasingType::EASE_IN},
  {0.15f, {EyeState{.lid_top=0.35f, .lid_bottom=0.20f}, F_LID_TOP|F_LID_BOTTOM}, EasingType::EASE_OUT},
  {0.22f, {EyeState{}, 0}, EasingType::EASE_OUT},
  {0.28f, {EyeState{}, 0}, EasingType::LINEAR},
  {0.33f, {EyeState{.lid_top=0.85f, .lid_bottom=0.50f}, F_LID_TOP|F_LID_BOTTOM}, EasingType::EASE_IN},
  {0.37f, {EyeState{.lid_top=1.0f, .lid_bottom=0.75f}, F_LID_TOP|F_LID_BOTTOM}, EasingType::EASE_IN},
  {0.44f, {EyeState{.lid_top=0.35f, .lid_bottom=0.20f}, F_LID_TOP|F_LID_BOTTOM}, EasingType::EASE_OUT},
  {0.55f, {EyeState{}, 0}, EasingType::EASE_OUT},
};

// ============================================================
// look_left — 1.2s, 6 keyframes
// ============================================================
static const Keyframe kf_look_left[] = {
  {0.0f,  {EyeState{.pupil_width=0.35f, .gaze_x=0.0f, .gaze_y=0.0f}, F_PUPIL_WIDTH|F_GAZE_X|F_GAZE_Y}, EasingType::LINEAR},
  {0.08f, {EyeState{.pupil_width=0.30f, .gaze_x=-0.75f, .gaze_y=-0.06f}, F_PUPIL_WIDTH|F_GAZE_X|F_GAZE_Y}, EasingType::EASE_OUT},
  {0.18f, {EyeState{.pupil_width=0.33f, .gaze_x=-0.65f, .gaze_y=-0.04f}, F_PUPIL_WIDTH|F_GAZE_X|F_GAZE_Y}, EasingType::EASE_OUT},
  {0.9f,  {EyeState{.pupil_width=0.35f, .gaze_x=-0.65f, .gaze_y=-0.02f}, F_PUPIL_WIDTH|F_GAZE_X|F_GAZE_Y}, EasingType::EASE_IN_OUT},
  {1.05f, {EyeState{.pupil_width=0.33f, .gaze_x=-0.08f, .gaze_y=0.02f}, F_PUPIL_WIDTH|F_GAZE_X|F_GAZE_Y}, EasingType::EASE_IN_OUT},
  {1.2f,  {EyeState{.pupil_width=0.35f, .gaze_x=0.0f, .gaze_y=0.0f}, F_PUPIL_WIDTH|F_GAZE_X|F_GAZE_Y}, EasingType::EASE_OUT},
};

// ============================================================
// look_right — 1.2s, 6 keyframes
// ============================================================
static const Keyframe kf_look_right[] = {
  {0.0f,  {EyeState{.pupil_width=0.35f, .gaze_x=0.0f, .gaze_y=0.0f}, F_PUPIL_WIDTH|F_GAZE_X|F_GAZE_Y}, EasingType::LINEAR},
  {0.08f, {EyeState{.pupil_width=0.30f, .gaze_x=0.75f, .gaze_y=-0.06f}, F_PUPIL_WIDTH|F_GAZE_X|F_GAZE_Y}, EasingType::EASE_OUT},
  {0.18f, {EyeState{.pupil_width=0.33f, .gaze_x=0.65f, .gaze_y=-0.04f}, F_PUPIL_WIDTH|F_GAZE_X|F_GAZE_Y}, EasingType::EASE_OUT},
  {0.9f,  {EyeState{.pupil_width=0.35f, .gaze_x=0.65f, .gaze_y=-0.02f}, F_PUPIL_WIDTH|F_GAZE_X|F_GAZE_Y}, EasingType::EASE_IN_OUT},
  {1.05f, {EyeState{.pupil_width=0.33f, .gaze_x=0.08f, .gaze_y=0.02f}, F_PUPIL_WIDTH|F_GAZE_X|F_GAZE_Y}, EasingType::EASE_IN_OUT},
  {1.2f,  {EyeState{.pupil_width=0.35f, .gaze_x=0.0f, .gaze_y=0.0f}, F_PUPIL_WIDTH|F_GAZE_X|F_GAZE_Y}, EasingType::EASE_OUT},
};

// ============================================================
// look_around — 2.8s, 14 keyframes
// ============================================================
static const Keyframe kf_look_around[] = {
  {0.0f,  {EyeState{.gaze_x=0.0f, .gaze_y=0.0f}, F_GAZE_X|F_GAZE_Y}, EasingType::LINEAR},
  {0.10f, {EyeState{.gaze_x=-0.6f, .gaze_y=-0.2f}, F_GAZE_X|F_GAZE_Y}, EasingType::EASE_OUT},
  {0.20f, {EyeState{.gaze_x=-0.55f, .gaze_y=-0.15f}, F_GAZE_X|F_GAZE_Y}, EasingType::EASE_OUT},
  {0.6f,  {EyeState{.gaze_x=-0.55f, .gaze_y=-0.12f}, F_GAZE_X|F_GAZE_Y}, EasingType::EASE_IN_OUT},
  // Blink during transition
  {0.70f, {EyeState{.gaze_x=-0.3f, .gaze_y=0.0f, .lid_top=0.85f, .lid_bottom=0.50f}, F_GAZE_X|F_GAZE_Y|F_LID_TOP|F_LID_BOTTOM}, EasingType::EASE_IN},
  {0.80f, {EyeState{.gaze_x=0.1f, .gaze_y=0.0f}, F_GAZE_X|F_GAZE_Y}, EasingType::EASE_OUT},
  {0.95f, {EyeState{.gaze_x=0.65f, .gaze_y=-0.1f}, F_GAZE_X|F_GAZE_Y}, EasingType::EASE_OUT},
  {1.05f, {EyeState{.gaze_x=0.58f, .gaze_y=-0.08f}, F_GAZE_X|F_GAZE_Y}, EasingType::EASE_OUT},
  {1.6f,  {EyeState{.gaze_x=0.58f, .gaze_y=-0.05f}, F_GAZE_X|F_GAZE_Y}, EasingType::EASE_IN_OUT},
  {1.72f, {EyeState{.gaze_x=0.3f, .gaze_y=0.25f}, F_GAZE_X|F_GAZE_Y}, EasingType::EASE_OUT},
  {1.82f, {EyeState{.gaze_x=0.25f, .gaze_y=0.2f}, F_GAZE_X|F_GAZE_Y}, EasingType::EASE_OUT},
  {2.2f,  {EyeState{.gaze_x=0.25f, .gaze_y=0.18f}, F_GAZE_X|F_GAZE_Y}, EasingType::EASE_IN_OUT},
  {2.5f,  {EyeState{.gaze_x=-0.05f, .gaze_y=0.03f}, F_GAZE_X|F_GAZE_Y}, EasingType::EASE_IN_OUT},
  {2.8f,  {EyeState{.gaze_x=0.0f, .gaze_y=0.0f}, F_GAZE_X|F_GAZE_Y}, EasingType::EASE_OUT},
};

// ============================================================
// angry — 2.0s, 6 keyframes
// ============================================================
static const Keyframe kf_angry[] = {
  {0.0f,  {EyeState{}, 0}, EasingType::LINEAR},
  {0.12f, {EyeState{.pupil_width=0.15f, .pupil_height=0.92f, .lid_top=0.45f, .lid_bottom=0.30f, .lid_top_bend=0.75f, .lid_bottom_bend=0.40f, .eye_openness=0.78f},
    F_PUPIL_WIDTH|F_PUPIL_HEIGHT|F_LID_TOP|F_LID_BOTTOM|F_LID_TOP_BEND|F_LID_BOTTOM_BEND|F_EYE_OPENNESS}, EasingType::EASE_OUT},
  {0.4f,  {EyeState{.pupil_width=0.12f, .pupil_height=0.95f, .gaze_y=-0.04f, .lid_top=0.50f, .lid_bottom=0.35f, .lid_top_bend=0.85f, .lid_bottom_bend=0.45f, .eye_openness=0.75f},
    F_PUPIL_WIDTH|F_PUPIL_HEIGHT|F_GAZE_Y|F_LID_TOP|F_LID_BOTTOM|F_LID_TOP_BEND|F_LID_BOTTOM_BEND|F_EYE_OPENNESS}, EasingType::EASE_IN_OUT},
  {1.2f,  {EyeState{.pupil_width=0.14f, .pupil_height=0.93f, .gaze_y=-0.02f, .lid_top=0.48f, .lid_bottom=0.32f, .lid_top_bend=0.82f, .lid_bottom_bend=0.42f, .eye_openness=0.76f},
    F_PUPIL_WIDTH|F_PUPIL_HEIGHT|F_GAZE_Y|F_LID_TOP|F_LID_BOTTOM|F_LID_TOP_BEND|F_LID_BOTTOM_BEND|F_EYE_OPENNESS}, EasingType::EASE_IN_OUT},
  {1.6f,  {EyeState{.pupil_width=0.28f, .lid_top=0.30f, .lid_bottom=0.22f, .lid_top_bend=0.55f, .lid_bottom_bend=0.32f, .eye_openness=0.85f},
    F_PUPIL_WIDTH|F_LID_TOP|F_LID_BOTTOM|F_LID_TOP_BEND|F_LID_BOTTOM_BEND|F_EYE_OPENNESS}, EasingType::EASE_IN_OUT},
  {2.0f,  {EyeState{}, 0}, EasingType::EASE_OUT},
};

// ============================================================
// wide — 1.85s, 5 keyframes
// ============================================================
static const Keyframe kf_wide[] = {
  {0.0f,  {EyeState{}, 0}, EasingType::LINEAR},
  {0.15f, {EyeState{.pupil_width=0.40f, .lid_top=0.04f, .lid_bottom=0.03f, .highlight_size=0.20f, .eye_openness=1.0f},
    F_PUPIL_WIDTH|F_LID_TOP|F_LID_BOTTOM|F_HIGHLIGHT_SIZE|F_EYE_OPENNESS}, EasingType::EASE_OUT},
  {1.5f,  {EyeState{.pupil_width=0.38f, .lid_top=0.04f, .lid_bottom=0.03f, .highlight_size=0.19f, .eye_openness=1.0f},
    F_PUPIL_WIDTH|F_LID_TOP|F_LID_BOTTOM|F_HIGHLIGHT_SIZE|F_EYE_OPENNESS}, EasingType::EASE_IN_OUT},
  {1.65f, {EyeState{.lid_top=0.80f, .lid_bottom=0.45f}, F_LID_TOP|F_LID_BOTTOM}, EasingType::EASE_IN},
  {1.85f, {EyeState{}, 0}, EasingType::EASE_OUT},
};

// ============================================================
// surprised — 1.3s, 7 keyframes
// ============================================================
static const Keyframe kf_surprised[] = {
  {0.0f,  {EyeState{}, 0}, EasingType::LINEAR},
  {0.08f, {EyeState{.pupil_width=0.7f, .pupil_height=0.88f, .gaze_y=-0.06f, .iris_size=0.94f, .lid_top=0.03f, .lid_bottom=0.02f, .highlight_size=0.22f, .eye_openness=1.0f},
    F_PUPIL_WIDTH|F_PUPIL_HEIGHT|F_GAZE_Y|F_IRIS_SIZE|F_LID_TOP|F_LID_BOTTOM|F_HIGHLIGHT_SIZE|F_EYE_OPENNESS}, EasingType::EASE_OUT_BACK},
  {0.25f, {EyeState{.pupil_width=0.75f, .pupil_height=0.9f, .gaze_y=-0.03f, .iris_size=0.93f, .lid_top=0.03f, .lid_bottom=0.02f, .highlight_size=0.21f, .eye_openness=1.0f},
    F_PUPIL_WIDTH|F_PUPIL_HEIGHT|F_GAZE_Y|F_IRIS_SIZE|F_LID_TOP|F_LID_BOTTOM|F_HIGHLIGHT_SIZE|F_EYE_OPENNESS}, EasingType::EASE_OUT},
  {0.8f,  {EyeState{.pupil_width=0.72f, .pupil_height=0.89f, .gaze_y=-0.02f, .lid_top=0.05f, .lid_bottom=0.03f, .highlight_size=0.20f, .eye_openness=1.0f},
    F_PUPIL_WIDTH|F_PUPIL_HEIGHT|F_GAZE_Y|F_LID_TOP|F_LID_BOTTOM|F_HIGHLIGHT_SIZE|F_EYE_OPENNESS}, EasingType::EASE_IN_OUT},
  {0.92f, {EyeState{.pupil_width=0.5f, .lid_top=0.75f, .lid_bottom=0.40f}, F_PUPIL_WIDTH|F_LID_TOP|F_LID_BOTTOM}, EasingType::EASE_IN},
  {1.1f,  {EyeState{.pupil_width=0.40f}, F_PUPIL_WIDTH}, EasingType::EASE_OUT},
  {1.3f,  {EyeState{}, 0}, EasingType::EASE_OUT},
};

// ============================================================
// sleepy — 3.5s, 6 keyframes, looping
// ============================================================
static const Keyframe kf_sleepy[] = {
  {0.0f,  {EyeState{.pupil_width=0.42f, .gaze_x=0.0f, .gaze_y=0.12f, .lid_top=0.52f, .lid_bottom=0.28f, .eye_openness=0.72f},
    F_PUPIL_WIDTH|F_GAZE_X|F_GAZE_Y|F_LID_TOP|F_LID_BOTTOM|F_EYE_OPENNESS}, EasingType::LINEAR},
  {1.2f,  {EyeState{.pupil_width=0.46f, .gaze_x=-0.06f, .gaze_y=0.18f, .lid_top=0.58f, .lid_bottom=0.32f, .eye_openness=0.65f},
    F_PUPIL_WIDTH|F_GAZE_X|F_GAZE_Y|F_LID_TOP|F_LID_BOTTOM|F_EYE_OPENNESS}, EasingType::EASE_IN_OUT},
  {2.0f,  {EyeState{.pupil_width=0.50f, .gaze_x=-0.08f, .gaze_y=0.22f, .lid_top=0.68f, .lid_bottom=0.38f, .eye_openness=0.55f},
    F_PUPIL_WIDTH|F_GAZE_X|F_GAZE_Y|F_LID_TOP|F_LID_BOTTOM|F_EYE_OPENNESS}, EasingType::EASE_IN_OUT},
  // Micro-jolt
  {2.15f, {EyeState{.pupil_width=0.38f, .gaze_x=0.0f, .gaze_y=0.05f, .lid_top=0.40f, .lid_bottom=0.22f, .eye_openness=0.78f},
    F_PUPIL_WIDTH|F_GAZE_X|F_GAZE_Y|F_LID_TOP|F_LID_BOTTOM|F_EYE_OPENNESS}, EasingType::EASE_OUT},
  {2.6f,  {EyeState{.pupil_width=0.44f, .gaze_x=0.03f, .gaze_y=0.14f, .lid_top=0.54f, .lid_bottom=0.30f, .eye_openness=0.70f},
    F_PUPIL_WIDTH|F_GAZE_X|F_GAZE_Y|F_LID_TOP|F_LID_BOTTOM|F_EYE_OPENNESS}, EasingType::EASE_IN_OUT},
  {3.5f,  {EyeState{.pupil_width=0.42f, .gaze_x=0.0f, .gaze_y=0.12f, .lid_top=0.52f, .lid_bottom=0.28f, .eye_openness=0.72f},
    F_PUPIL_WIDTH|F_GAZE_X|F_GAZE_Y|F_LID_TOP|F_LID_BOTTOM|F_EYE_OPENNESS}, EasingType::EASE_IN_OUT},
};

// ============================================================
// squint — 1.1s, 6 keyframes
// ============================================================
static const Keyframe kf_squint[] = {
  {0.0f,  {EyeState{}, 0}, EasingType::LINEAR},
  {0.10f, {EyeState{.pupil_width=0.22f, .gaze_y=-0.03f, .lid_top=0.38f, .lid_bottom=0.34f, .eye_openness=0.68f},
    F_PUPIL_WIDTH|F_GAZE_Y|F_LID_TOP|F_LID_BOTTOM|F_EYE_OPENNESS}, EasingType::EASE_OUT},
  {0.3f,  {EyeState{.pupil_width=0.20f, .gaze_y=-0.04f, .lid_top=0.42f, .lid_bottom=0.38f, .eye_openness=0.65f},
    F_PUPIL_WIDTH|F_GAZE_Y|F_LID_TOP|F_LID_BOTTOM|F_EYE_OPENNESS}, EasingType::EASE_IN_OUT},
  {0.7f,  {EyeState{.pupil_width=0.21f, .gaze_y=-0.03f, .lid_top=0.40f, .lid_bottom=0.36f, .eye_openness=0.66f},
    F_PUPIL_WIDTH|F_GAZE_Y|F_LID_TOP|F_LID_BOTTOM|F_EYE_OPENNESS}, EasingType::EASE_IN_OUT},
  {0.95f, {EyeState{.pupil_width=0.32f, .lid_top=0.26f, .lid_bottom=0.18f, .eye_openness=0.86f},
    F_PUPIL_WIDTH|F_LID_TOP|F_LID_BOTTOM|F_EYE_OPENNESS}, EasingType::EASE_IN_OUT},
  {1.1f,  {EyeState{}, 0}, EasingType::EASE_OUT},
};

// ============================================================
// wake_up — 2.7s, 8 keyframes
// ============================================================
static const Keyframe kf_wake_up[] = {
  {0.0f,  {EyeState{.pupil_width=0.65f, .lid_top=1.0f, .lid_bottom=0.90f, .eye_openness=0.3f},
    F_PUPIL_WIDTH|F_LID_TOP|F_LID_BOTTOM|F_EYE_OPENNESS}, EasingType::LINEAR},
  {0.5f,  {EyeState{.pupil_width=0.60f, .gaze_y=0.15f, .lid_top=0.75f, .lid_bottom=0.60f, .eye_openness=0.45f},
    F_PUPIL_WIDTH|F_GAZE_Y|F_LID_TOP|F_LID_BOTTOM|F_EYE_OPENNESS}, EasingType::EASE_OUT},
  {0.8f,  {EyeState{.pupil_width=0.62f, .gaze_y=0.10f, .lid_top=0.90f, .lid_bottom=0.80f, .eye_openness=0.35f},
    F_PUPIL_WIDTH|F_GAZE_Y|F_LID_TOP|F_LID_BOTTOM|F_EYE_OPENNESS}, EasingType::EASE_IN_OUT},
  {1.3f,  {EyeState{.pupil_width=0.50f, .gaze_y=0.08f, .lid_top=0.50f, .lid_bottom=0.35f, .eye_openness=0.60f},
    F_PUPIL_WIDTH|F_GAZE_Y|F_LID_TOP|F_LID_BOTTOM|F_EYE_OPENNESS}, EasingType::EASE_OUT},
  {1.8f,  {EyeState{.pupil_width=0.20f, .gaze_y=0.0f, .lid_top=0.40f, .lid_bottom=0.30f, .eye_openness=0.70f},
    F_PUPIL_WIDTH|F_GAZE_Y|F_LID_TOP|F_LID_BOTTOM|F_EYE_OPENNESS}, EasingType::EASE_IN_OUT},
  {2.3f,  {EyeState{.pupil_width=0.30f, .gaze_y=-0.03f, .lid_top=0.25f, .lid_bottom=0.17f, .eye_openness=0.88f},
    F_PUPIL_WIDTH|F_GAZE_Y|F_LID_TOP|F_LID_BOTTOM|F_EYE_OPENNESS}, EasingType::EASE_OUT},
  {2.5f,  {EyeState{.pupil_width=0.32f, .lid_top=0.75f, .lid_bottom=0.45f},
    F_PUPIL_WIDTH|F_LID_TOP|F_LID_BOTTOM}, EasingType::EASE_IN},
  {2.7f,  {EyeState{}, 0}, EasingType::EASE_OUT},
};

// ============================================================
// dilate — 1.0s, 5 keyframes
// ============================================================
static const Keyframe kf_dilate[] = {
  {0.0f,  {EyeState{.pupil_width=0.35f, .pupil_height=0.85f}, F_PUPIL_WIDTH|F_PUPIL_HEIGHT}, EasingType::LINEAR},
  {0.15f, {EyeState{.pupil_width=0.80f, .pupil_height=0.90f}, F_PUPIL_WIDTH|F_PUPIL_HEIGHT}, EasingType::EASE_OUT_BACK},
  {0.3f,  {EyeState{.pupil_width=0.85f, .pupil_height=0.92f}, F_PUPIL_WIDTH|F_PUPIL_HEIGHT}, EasingType::EASE_OUT},
  {0.7f,  {EyeState{.pupil_width=0.82f, .pupil_height=0.91f}, F_PUPIL_WIDTH|F_PUPIL_HEIGHT}, EasingType::EASE_IN_OUT},
  {1.0f,  {EyeState{.pupil_width=0.35f, .pupil_height=0.85f}, F_PUPIL_WIDTH|F_PUPIL_HEIGHT}, EasingType::EASE_IN_OUT},
};

// ============================================================
// constrict — 0.8s, 5 keyframes
// ============================================================
static const Keyframe kf_constrict[] = {
  {0.0f,  {EyeState{.pupil_width=0.35f, .pupil_height=0.85f}, F_PUPIL_WIDTH|F_PUPIL_HEIGHT}, EasingType::LINEAR},
  {0.10f, {EyeState{.pupil_width=0.08f, .pupil_height=0.80f}, F_PUPIL_WIDTH|F_PUPIL_HEIGHT}, EasingType::EASE_OUT},
  {0.20f, {EyeState{.pupil_width=0.06f, .pupil_height=0.78f}, F_PUPIL_WIDTH|F_PUPIL_HEIGHT}, EasingType::EASE_OUT},
  {0.5f,  {EyeState{.pupil_width=0.07f, .pupil_height=0.79f}, F_PUPIL_WIDTH|F_PUPIL_HEIGHT}, EasingType::EASE_IN_OUT},
  {0.8f,  {EyeState{.pupil_width=0.35f, .pupil_height=0.85f}, F_PUPIL_WIDTH|F_PUPIL_HEIGHT}, EasingType::EASE_IN_OUT},
};

// ============================================================
// cute — 5.3s, 14 keyframes
// ============================================================
static const Keyframe kf_cute[] = {
  {0.0f,  {EyeState{}, 0}, EasingType::LINEAR},
  {0.15f, {EyeState{.pupil_width=0.88f, .pupil_height=0.92f, .gaze_x=0.0f, .gaze_y=-0.12f, .iris_size=0.94f, .lid_top=0.04f, .lid_bottom=0.03f, .highlight_size=0.26f, .highlight_y=-0.28f, .eye_openness=1.0f},
    F_PUPIL_WIDTH|F_PUPIL_HEIGHT|F_GAZE_X|F_GAZE_Y|F_IRIS_SIZE|F_LID_TOP|F_LID_BOTTOM|F_HIGHLIGHT_SIZE|F_HIGHLIGHT_Y|F_EYE_OPENNESS}, EasingType::EASE_OUT_BACK},
  {0.4f,  {EyeState{.pupil_width=0.92f, .pupil_height=0.94f, .gaze_x=0.02f, .gaze_y=-0.15f, .iris_size=0.95f, .lid_top=0.03f, .lid_bottom=0.03f, .highlight_size=0.28f, .highlight_y=-0.26f, .eye_openness=1.0f},
    F_PUPIL_WIDTH|F_PUPIL_HEIGHT|F_GAZE_X|F_GAZE_Y|F_IRIS_SIZE|F_LID_TOP|F_LID_BOTTOM|F_HIGHLIGHT_SIZE|F_HIGHLIGHT_Y|F_EYE_OPENNESS}, EasingType::EASE_OUT},
  // Wobble right
  {1.0f,  {EyeState{.pupil_width=0.90f, .pupil_height=0.93f, .gaze_x=0.08f, .gaze_y=-0.13f, .highlight_size=0.27f},
    F_PUPIL_WIDTH|F_PUPIL_HEIGHT|F_GAZE_X|F_GAZE_Y|F_HIGHLIGHT_SIZE}, EasingType::EASE_IN_OUT},
  // Pleading blink
  {1.3f,  {EyeState{.pupil_width=0.93f, .gaze_x=0.04f, .gaze_y=-0.10f, .lid_top=0.50f, .lid_bottom=0.25f},
    F_PUPIL_WIDTH|F_GAZE_X|F_GAZE_Y|F_LID_TOP|F_LID_BOTTOM}, EasingType::EASE_IN_OUT},
  {1.55f, {EyeState{.pupil_width=0.92f, .pupil_height=0.94f, .gaze_x=0.0f, .gaze_y=-0.16f, .lid_top=0.03f, .lid_bottom=0.03f, .highlight_size=0.29f, .eye_openness=1.0f},
    F_PUPIL_WIDTH|F_PUPIL_HEIGHT|F_GAZE_X|F_GAZE_Y|F_LID_TOP|F_LID_BOTTOM|F_HIGHLIGHT_SIZE|F_EYE_OPENNESS}, EasingType::EASE_OUT},
  // Wobble left
  {2.1f,  {EyeState{.pupil_width=0.90f, .gaze_x=-0.07f, .gaze_y=-0.14f, .highlight_size=0.27f},
    F_PUPIL_WIDTH|F_GAZE_X|F_GAZE_Y|F_HIGHLIGHT_SIZE}, EasingType::EASE_IN_OUT},
  {2.6f,  {EyeState{.pupil_width=0.91f, .gaze_x=0.03f, .gaze_y=-0.15f, .highlight_size=0.28f},
    F_PUPIL_WIDTH|F_GAZE_X|F_GAZE_Y|F_HIGHLIGHT_SIZE}, EasingType::EASE_IN_OUT},
  // Second pleading blink
  {3.0f,  {EyeState{.pupil_width=0.93f, .gaze_y=-0.11f, .lid_top=0.45f, .lid_bottom=0.22f},
    F_PUPIL_WIDTH|F_GAZE_Y|F_LID_TOP|F_LID_BOTTOM}, EasingType::EASE_IN_OUT},
  {3.25f, {EyeState{.pupil_width=0.92f, .gaze_y=-0.16f, .lid_top=0.03f, .lid_bottom=0.03f, .highlight_size=0.30f, .eye_openness=1.0f},
    F_PUPIL_WIDTH|F_GAZE_Y|F_LID_TOP|F_LID_BOTTOM|F_HIGHLIGHT_SIZE|F_EYE_OPENNESS}, EasingType::EASE_OUT},
  {4.0f,  {EyeState{.pupil_width=0.91f, .gaze_x=-0.03f, .gaze_y=-0.14f, .highlight_size=0.28f},
    F_PUPIL_WIDTH|F_GAZE_X|F_GAZE_Y|F_HIGHLIGHT_SIZE}, EasingType::EASE_IN_OUT},
  // Return to almond
  {4.8f,  {EyeState{.pupil_width=0.60f, .pupil_height=0.88f, .gaze_x=0.0f, .gaze_y=-0.05f, .iris_size=0.93f, .highlight_size=0.22f, .highlight_y=-0.30f, .eye_openness=0.92f},
    F_PUPIL_WIDTH|F_PUPIL_HEIGHT|F_GAZE_X|F_GAZE_Y|F_IRIS_SIZE|F_HIGHLIGHT_SIZE|F_HIGHLIGHT_Y|F_EYE_OPENNESS}, EasingType::EASE_IN_OUT},
  {5.0f,  {EyeState{.lid_top=0.65f, .lid_bottom=0.35f}, F_LID_TOP|F_LID_BOTTOM}, EasingType::EASE_IN},
  {5.3f,  {EyeState{}, 0}, EasingType::EASE_OUT},
};

// ============================================================
// idle — 5.5s, 7 keyframes, looping
// ============================================================
static const Keyframe kf_idle[] = {
  {0.0f,  {EyeState{.pupil_width=0.35f, .gaze_x=0.0f, .gaze_y=0.0f, .highlight_x=-0.25f},
    F_PUPIL_WIDTH|F_GAZE_X|F_GAZE_Y|F_HIGHLIGHT_X}, EasingType::LINEAR},
  {0.8f,  {EyeState{.pupil_width=0.36f, .gaze_x=0.04f, .gaze_y=-0.02f, .highlight_x=-0.24f},
    F_PUPIL_WIDTH|F_GAZE_X|F_GAZE_Y|F_HIGHLIGHT_X}, EasingType::EASE_IN_OUT},
  {1.8f,  {EyeState{.pupil_width=0.34f, .gaze_x=-0.06f, .gaze_y=0.03f, .highlight_x=-0.26f},
    F_PUPIL_WIDTH|F_GAZE_X|F_GAZE_Y|F_HIGHLIGHT_X}, EasingType::EASE_IN_OUT},
  {2.8f,  {EyeState{.pupil_width=0.37f, .gaze_x=0.03f, .gaze_y=0.05f, .highlight_x=-0.23f},
    F_PUPIL_WIDTH|F_GAZE_X|F_GAZE_Y|F_HIGHLIGHT_X}, EasingType::EASE_IN_OUT},
  {3.6f,  {EyeState{.pupil_width=0.34f, .gaze_x=-0.02f, .gaze_y=-0.04f, .highlight_x=-0.26f},
    F_PUPIL_WIDTH|F_GAZE_X|F_GAZE_Y|F_HIGHLIGHT_X}, EasingType::EASE_IN_OUT},
  {4.5f,  {EyeState{.pupil_width=0.36f, .gaze_x=0.05f, .gaze_y=0.01f, .highlight_x=-0.24f},
    F_PUPIL_WIDTH|F_GAZE_X|F_GAZE_Y|F_HIGHLIGHT_X}, EasingType::EASE_IN_OUT},
  {5.5f,  {EyeState{.pupil_width=0.35f, .gaze_x=0.0f, .gaze_y=0.0f, .highlight_x=-0.25f},
    F_PUPIL_WIDTH|F_GAZE_X|F_GAZE_Y|F_HIGHLIGHT_X}, EasingType::EASE_IN_OUT},
};

// ============================================================
// Animation table
// ============================================================
static const Animation ALL_ANIMATIONS[] = {
  {"blink",        kf_blink,        sizeof(kf_blink)/sizeof(kf_blink[0]),               false},
  {"slow_blink",   kf_slow_blink,   sizeof(kf_slow_blink)/sizeof(kf_slow_blink[0]),     false},
  {"double_blink", kf_double_blink, sizeof(kf_double_blink)/sizeof(kf_double_blink[0]), false},
  {"look_left",    kf_look_left,    sizeof(kf_look_left)/sizeof(kf_look_left[0]),       false},
  {"look_right",   kf_look_right,   sizeof(kf_look_right)/sizeof(kf_look_right[0]),     false},
  {"look_around",  kf_look_around,  sizeof(kf_look_around)/sizeof(kf_look_around[0]),   false},
  {"angry",        kf_angry,        sizeof(kf_angry)/sizeof(kf_angry[0]),               false},
  {"wide",         kf_wide,         sizeof(kf_wide)/sizeof(kf_wide[0]),                 false},
  {"surprised",    kf_surprised,    sizeof(kf_surprised)/sizeof(kf_surprised[0]),       false},
  {"sleepy",       kf_sleepy,       sizeof(kf_sleepy)/sizeof(kf_sleepy[0]),             true},
  {"squint",       kf_squint,       sizeof(kf_squint)/sizeof(kf_squint[0]),             false},
  {"wake_up",      kf_wake_up,      sizeof(kf_wake_up)/sizeof(kf_wake_up[0]),           false},
  {"dilate",       kf_dilate,       sizeof(kf_dilate)/sizeof(kf_dilate[0]),             false},
  {"constrict",    kf_constrict,    sizeof(kf_constrict)/sizeof(kf_constrict[0]),       false},
  {"cute",         kf_cute,         sizeof(kf_cute)/sizeof(kf_cute[0]),                 false},
  {"idle",         kf_idle,         sizeof(kf_idle)/sizeof(kf_idle[0]),                 true},
};

static constexpr int ANIMATION_COUNT = sizeof(ALL_ANIMATIONS) / sizeof(ALL_ANIMATIONS[0]);

inline const Animation* find_animation(const char* name) {
  for (int i = 0; i < ANIMATION_COUNT; i++) {
    if (strcmp(ALL_ANIMATIONS[i].name, name) == 0) {
      return &ALL_ANIMATIONS[i];
    }
  }
  return nullptr;
}

inline int get_animation_count() { return ANIMATION_COUNT; }
inline const Animation* get_animation(int index) {
  if (index < 0 || index >= ANIMATION_COUNT) return nullptr;
  return &ALL_ANIMATIONS[index];
}

}  // namespace cat_eyes
