#include "cat_eyes.h"
#include "esphome/core/log.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"

static const char* TAG = "cat_eyes";

namespace cat_eyes {

void CatEyes::setup() {
  ESP_LOGI(TAG, "Initializing Cat Eyes...");

  // Initialize SPI bus (we own it directly for DMA performance)
  spi_bus_config_t bus_cfg = {};
  bus_cfg.mosi_io_num = mosi_pin_;
  bus_cfg.miso_io_num = -1;
  bus_cfg.sclk_io_num = clk_pin_;
  bus_cfg.quadwp_io_num = -1;
  bus_cfg.quadhd_io_num = -1;
  bus_cfg.max_transfer_sz = DISPLAY_WIDTH * 2 + 8;

  esp_err_t ret = spi_bus_initialize(SPI3_HOST, &bus_cfg, SPI_DMA_CH_AUTO);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "SPI bus init failed: %d", ret);
    return;
  }

  // Initialize displays (shared DC and reset pins)
  ESP_LOGI(TAG, "Initializing right display (CS=%d)...", cs_right_pin_);
  display_right_.init(SPI3_HOST, cs_right_pin_, dc_pin_, reset_pin_, spi_frequency_);

  ESP_LOGI(TAG, "Initializing left display (CS=%d)...", cs_left_pin_);
  display_left_.init(SPI3_HOST, cs_left_pin_, dc_pin_, -1, spi_frequency_);  // shared reset already done

  // Initialize palette (default amber)
  palette_.set_from_rgb(PALETTE_AMBER[0], PALETTE_AMBER[1], PALETTE_AMBER[2]);

  // Initialize renderer
  renderer_.init(&palette_);

  // Set initial state
  current_state_ = DEFAULT_STATE;

  // Initialize behavior system
  behavior_.init(&player_);

  // Fill screens with black initially
  display_right_.fill_screen(palette_.bg_color);
  display_left_.fill_screen(palette_.bg_color);

  // Create render task pinned to Core 1
  xTaskCreatePinnedToCore(
    render_task,
    "cat_eyes_render",
    8192,
    this,
    5,
    &render_task_handle_,
    1
  );

  ESP_LOGI(TAG, "Cat Eyes initialized successfully");
}

void CatEyes::loop() {
  // Main loop on Core 0 — HA communication handled by ESPHome framework
}

void CatEyes::render_task(void* param) {
  auto* self = static_cast<CatEyes*>(param);
  TickType_t last_wake = xTaskGetTickCount();

  while (true) {
    // 1. Update animation state
    EyeState state;
    bool has_anim = self->player_.update(state);

    if (has_anim) {
      self->current_state_ = state;
    } else {
      // No active animation — run behavior (may start new animation)
      self->behavior_.update(self->current_state_);
      // Check again if behavior started something
      has_anim = self->player_.update(state);
      if (has_anim) {
        self->current_state_ = state;
      }
    }

    // Apply gaze override if active
    if (self->gaze_override_active_) {
      self->current_state_.gaze_x = self->gaze_override_x_;
      self->current_state_.gaze_y = self->gaze_override_y_;
    }

    // 2. Advance shimmer timer once per frame
    self->renderer_.tick();

    // 3. Render right eye
    self->renderer_.render_frame(&self->display_right_, self->current_state_, false);

    // 4. Render left eye (mirrored)
    self->renderer_.render_frame(&self->display_left_, self->current_state_, true);

    // 5. Yield to maintain frame timing
    vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(16));
  }
}

void CatEyes::play_animation(const std::string& name) {
  ESP_LOGI(TAG, "Playing animation: %s", name.c_str());
  const Animation* anim = find_animation(name.c_str());
  if (anim != nullptr) {
    player_.play(*anim, current_state_);
  } else {
    ESP_LOGW(TAG, "Unknown animation: %s", name.c_str());
  }
}

void CatEyes::set_gaze(float x, float y) {
  ESP_LOGI(TAG, "Setting gaze: %.2f, %.2f", x, y);
  gaze_override_x_ = x;
  gaze_override_y_ = y;
  gaze_override_active_ = true;
}

void CatEyes::set_palette(const std::string& preset) {
  ESP_LOGI(TAG, "Setting palette: %s", preset.c_str());
  palette_.set_preset(preset.c_str());
}

void CatEyes::set_palette_rgb(int r, int g, int b) {
  ESP_LOGI(TAG, "Setting palette RGB: %d, %d, %d", r, g, b);
  palette_.set_from_rgb((uint8_t)r, (uint8_t)g, (uint8_t)b);
}

void CatEyes::set_auto_idle(bool enabled) {
  ESP_LOGI(TAG, "Auto idle: %s", enabled ? "ON" : "OFF");
  behavior_.set_auto_idle(enabled);
}

}  // namespace cat_eyes
