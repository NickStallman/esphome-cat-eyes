#pragma once
#include "esphome/core/component.h"
#include "gc9a01.h"
#include "renderer.h"
#include "animation.h"
#include "animations.h"
#include "palette.h"
#include "eye_state.h"
#include <string>

namespace cat_eyes {

class CatEyes : public esphome::Component {
 public:
  void setup() override;
  void loop() override;
  float get_setup_priority() const override { return esphome::setup_priority::HARDWARE; }

  // Pin configuration (called from __init__.py codegen)
  void set_clk_pin(int pin) { clk_pin_ = pin; }
  void set_mosi_pin(int pin) { mosi_pin_ = pin; }
  void set_cs_right(int pin) { cs_right_pin_ = pin; }
  void set_cs_left(int pin) { cs_left_pin_ = pin; }
  void set_dc_pin(int pin) { dc_pin_ = pin; }
  void set_reset_pin(int pin) { reset_pin_ = pin; }
  void set_spi_frequency(int freq) { spi_frequency_ = freq; }

  // HA service handlers
  void play_animation(const std::string& name);
  void set_gaze(float x, float y);
  void set_palette(const std::string& preset);
  void set_palette_rgb(int r, int g, int b);
  void set_auto_idle(bool enabled);

 private:
  // Pin configuration
  int clk_pin_ = 18;
  int mosi_pin_ = 23;
  int cs_right_pin_ = 5;
  int cs_left_pin_ = 4;
  int dc_pin_ = 22;
  int reset_pin_ = 21;
  int spi_frequency_ = 80000000;

  // Hardware
  GC9A01Driver display_right_;
  GC9A01Driver display_left_;

  // Rendering
  EyeRenderer renderer_;
  ColorPalette palette_;

  // Animation
  AnimationPlayer player_;
  EyeBehavior behavior_;
  EyeState current_state_;

  // Render task on Core 1
  TaskHandle_t render_task_handle_ = nullptr;
  static void render_task(void* param);

  // Thread-safe gaze override
  volatile float gaze_override_x_ = 0.0f;
  volatile float gaze_override_y_ = 0.0f;
  volatile bool gaze_override_active_ = false;
};

}  // namespace cat_eyes
