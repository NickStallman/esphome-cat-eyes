#pragma once
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include "eye_state.h"
#include "easing.h"
#include "esp_timer.h"

namespace cat_eyes {

// Get monotonic time in seconds (float)
inline float monotonic_s() {
  return (float)(esp_timer_get_time() / 1000) / 1000.0f;
}

struct Keyframe {
  float time;
  KeyframeState kf_state;
  EasingType easing;
};

struct Animation {
  const char* name;
  const Keyframe* keyframes;
  uint8_t keyframe_count;
  bool loop;

  float duration() const {
    if (keyframe_count == 0) return 0.0f;
    return keyframes[keyframe_count - 1].time;
  }
};

// Maximum keyframes per animation in our library
static constexpr int MAX_KEYFRAMES = 16;
// Maximum resolved states cached
static constexpr int MAX_RESOLVED = 16;

class AnimationPlayer {
 public:
  void play(const Animation& anim, const EyeState& base_state) {
    animation_ = &anim;
    base_state_ = base_state;
    start_time_ = monotonic_s();
    active_ = true;

    // Pre-resolve all keyframes against base state
    int count = anim.keyframe_count;
    if (count > MAX_RESOLVED) count = MAX_RESOLVED;
    for (int i = 0; i < count; i++) {
      resolved_[i] = resolve_keyframe(base_state, anim.keyframes[i].kf_state);
    }
    resolved_count_ = count;
  }

  void stop() {
    active_ = false;
    animation_ = nullptr;
  }

  bool active() const { return active_; }

  const char* name() const {
    return (animation_ != nullptr) ? animation_->name : "";
  }

  // Returns true if state was updated, false if animation is inactive
  bool update(EyeState& out) {
    if (!active_ || animation_ == nullptr) return false;

    float elapsed = monotonic_s() - start_time_;
    float dur = animation_->duration();

    if (dur <= 0.0f) {
      active_ = false;
      return false;
    }

    if (animation_->loop) {
      elapsed = fmodf(elapsed, dur);
    } else if (elapsed >= dur) {
      active_ = false;
      out = resolved_[resolved_count_ - 1];
      return true;
    }

    const Keyframe* kfs = animation_->keyframes;
    for (int i = 0; i < resolved_count_ - 1; i++) {
      if (elapsed <= kfs[i + 1].time) {
        float seg_start = kfs[i].time;
        float seg_end = kfs[i + 1].time;
        float seg_dur = seg_end - seg_start;
        if (seg_dur <= 0.0f) {
          out = resolved_[i + 1];
          return true;
        }
        float raw_t = (elapsed - seg_start) / seg_dur;
        if (raw_t < 0.0f) raw_t = 0.0f;
        if (raw_t > 1.0f) raw_t = 1.0f;
        float t = apply_easing(kfs[i + 1].easing, raw_t);
        out = lerp_state(resolved_[i], resolved_[i + 1], t);
        return true;
      }
    }

    out = resolved_[resolved_count_ - 1];
    return true;
  }

 private:
  const Animation* animation_ = nullptr;
  EyeState base_state_;
  float start_time_ = 0.0f;
  bool active_ = false;
  EyeState resolved_[MAX_RESOLVED];
  int resolved_count_ = 0;
};

// Forward declaration — defined in animations.h
const Animation* find_animation(const char* name);
int get_animation_count();
const Animation* get_animation(int index);

class EyeBehavior {
 public:
  void init(AnimationPlayer* player) {
    player_ = player;
    auto_idle_ = true;
    float now = monotonic_s();
    last_blink_time_ = now;
    next_blink_interval_ = random_blink_interval();
    last_glance_time_ = now;
    next_glance_interval_ = random_glance_interval();
    idle_playing_ = false;
  }

  void set_auto_idle(bool enabled) { auto_idle_ = enabled; }
  bool auto_idle() const { return auto_idle_; }

  void update(const EyeState& current_state) {
    if (!auto_idle_) return;

    float now = monotonic_s();

    // Random blinks
    if (now - last_blink_time_ > next_blink_interval_) {
      if (!player_->active() || strcmp(player_->name(), "idle") == 0) {
        // Weighted random: 70% blink, 15% double_blink, 15% slow_blink
        int r = rand() % 100;
        const char* blink_type;
        if (r < 70) blink_type = "blink";
        else if (r < 85) blink_type = "double_blink";
        else blink_type = "slow_blink";

        trigger(blink_type, current_state);
        last_blink_time_ = now;
        next_blink_interval_ = random_blink_interval();
        return;
      }
    }

    // Random glances
    if (now - last_glance_time_ > next_glance_interval_) {
      if (!player_->active() || strcmp(player_->name(), "idle") == 0) {
        int r = rand() % 3;
        const char* glances[] = {"look_left", "look_right", "look_around"};
        trigger(glances[r], current_state);
        last_glance_time_ = now;
        next_glance_interval_ = random_glance_interval();
        return;
      }
    }

    // Start idle micro-movements if nothing is playing
    if (!player_->active() && !idle_playing_) {
      const Animation* idle_anim = find_animation("idle");
      if (idle_anim != nullptr) {
        player_->play(*idle_anim, current_state);
        idle_playing_ = true;
      }
    } else if (player_->active() && strcmp(player_->name(), "idle") != 0) {
      idle_playing_ = false;
    }
  }

  void trigger(const char* name, const EyeState& current_state) {
    const Animation* anim = find_animation(name);
    if (anim != nullptr) {
      player_->play(*anim, current_state);
      idle_playing_ = false;
      if (strcmp(name, "blink") == 0 || strcmp(name, "double_blink") == 0 ||
          strcmp(name, "slow_blink") == 0) {
        last_blink_time_ = monotonic_s();
      }
    }
  }

 private:
  AnimationPlayer* player_ = nullptr;
  bool auto_idle_ = true;
  float last_blink_time_ = 0.0f;
  float next_blink_interval_ = 3.0f;
  float last_glance_time_ = 0.0f;
  float next_glance_interval_ = 5.0f;
  bool idle_playing_ = false;

  static float random_blink_interval() {
    return 2.0f + (rand() % 3000) / 1000.0f;  // 2.0 - 5.0
  }
  static float random_glance_interval() {
    return 3.0f + (rand() % 5000) / 1000.0f;  // 3.0 - 8.0
  }
};

}  // namespace cat_eyes
