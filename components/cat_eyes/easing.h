#pragma once
#include <cmath>
#include <cstdint>

namespace cat_eyes {

enum class EasingType : uint8_t {
  LINEAR = 0,
  EASE_IN,
  EASE_OUT,
  EASE_IN_OUT,
  EASE_OUT_BACK,
  EASE_OUT_ELASTIC,
  EASE_IN_BACK,
};

inline float ease_linear(float t) { return t; }

inline float ease_in(float t) { return t * t * t; }

inline float ease_out(float t) {
  float inv = 1.0f - t;
  return 1.0f - inv * inv * inv;
}

inline float ease_in_out(float t) {
  if (t < 0.5f)
    return 4.0f * t * t * t;
  float inv = -2.0f * t + 2.0f;
  return 1.0f - inv * inv * inv / 2.0f;
}

inline float ease_out_back(float t) {
  constexpr float c1 = 1.70158f;
  constexpr float c3 = c1 + 1.0f;
  float tm1 = t - 1.0f;
  return 1.0f + c3 * tm1 * tm1 * tm1 + c1 * tm1 * tm1;
}

inline float ease_out_elastic(float t) {
  if (t == 0.0f || t == 1.0f)
    return t;
  return powf(2.0f, -10.0f * t) * sinf((t * 10.0f - 0.75f) * (2.0f * M_PI / 3.0f)) + 1.0f;
}

inline float ease_in_back(float t) {
  constexpr float c1 = 1.70158f;
  constexpr float c3 = c1 + 1.0f;
  return c3 * t * t * t - c1 * t * t;
}

inline float apply_easing(EasingType type, float t) {
  switch (type) {
    case EasingType::LINEAR:           return ease_linear(t);
    case EasingType::EASE_IN:          return ease_in(t);
    case EasingType::EASE_OUT:         return ease_out(t);
    case EasingType::EASE_IN_OUT:      return ease_in_out(t);
    case EasingType::EASE_OUT_BACK:    return ease_out_back(t);
    case EasingType::EASE_OUT_ELASTIC: return ease_out_elastic(t);
    case EasingType::EASE_IN_BACK:     return ease_in_back(t);
    default:                           return t;
  }
}

}  // namespace cat_eyes
