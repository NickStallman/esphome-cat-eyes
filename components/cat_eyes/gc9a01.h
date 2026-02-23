#pragma once
#include <cstdint>
#include <cstring>
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

namespace cat_eyes {

static constexpr int DISPLAY_WIDTH = 240;
static constexpr int DISPLAY_HEIGHT = 240;

class GC9A01Driver {
 public:
  void init(spi_host_device_t host, int cs_pin, int dc_pin, int rst_pin, int freq_hz) {
    dc_pin_ = (gpio_num_t)dc_pin;
    cs_pin_ = (gpio_num_t)cs_pin;

    // Configure DC pin as GPIO output
    gpio_config_t io_conf = {};
    io_conf.pin_bit_mask = (1ULL << dc_pin);
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpio_config(&io_conf);

    // Configure reset pin
    if (rst_pin >= 0) {
      io_conf.pin_bit_mask = (1ULL << rst_pin);
      gpio_config(&io_conf);

      // Hardware reset
      gpio_set_level((gpio_num_t)rst_pin, 0);
      vTaskDelay(pdMS_TO_TICKS(20));
      gpio_set_level((gpio_num_t)rst_pin, 1);
      vTaskDelay(pdMS_TO_TICKS(120));
    }

    // SPI device configuration
    spi_device_interface_config_t dev_cfg = {};
    dev_cfg.clock_speed_hz = freq_hz;
    dev_cfg.mode = 0;
    dev_cfg.spics_io_num = cs_pin;
    dev_cfg.queue_size = 2;
    dev_cfg.flags = 0;

    esp_err_t ret = spi_bus_add_device(host, &dev_cfg, &spi_handle_);
    if (ret != ESP_OK) {
      ESP_LOGE("GC9A01", "Failed to add SPI device: %d", ret);
      return;
    }

    // GC9A01 initialization sequence
    init_display();
  }

  void send_command(uint8_t cmd) {
    gpio_set_level(dc_pin_, 0);
    spi_transaction_t t = {};
    t.length = 8;
    t.tx_data[0] = cmd;
    t.flags = SPI_TRANS_USE_TXDATA;
    spi_device_polling_transmit(spi_handle_, &t);
  }

  void send_data(const uint8_t* data, size_t len) {
    if (len == 0) return;
    gpio_set_level(dc_pin_, 1);
    spi_transaction_t t = {};
    t.length = len * 8;
    if (len <= 4) {
      memcpy(t.tx_data, data, len);
      t.flags = SPI_TRANS_USE_TXDATA;
    } else {
      t.tx_buffer = data;
    }
    spi_device_polling_transmit(spi_handle_, &t);
  }

  void send_data_byte(uint8_t val) {
    send_data(&val, 1);
  }

  void set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    uint8_t col_data[] = {(uint8_t)(x0 >> 8), (uint8_t)x0, (uint8_t)(x1 >> 8), (uint8_t)x1};
    uint8_t row_data[] = {(uint8_t)(y0 >> 8), (uint8_t)y0, (uint8_t)(y1 >> 8), (uint8_t)y1};

    send_command(0x2A);  // Column address set
    send_data(col_data, 4);
    send_command(0x2B);  // Row address set
    send_data(row_data, 4);
    send_command(0x2C);  // Memory write
  }

  void begin_scanline_write() {
    set_window(0, 0, DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 1);
    active_buf_ = 0;
  }

  void push_scanline(const uint16_t* buf, uint16_t len) {
    // Copy to DMA-capable buffer
    memcpy(line_buf_[active_buf_], buf, len * 2);

    gpio_set_level(dc_pin_, 1);
    spi_transaction_t t = {};
    t.length = len * 16;
    t.tx_buffer = line_buf_[active_buf_];
    spi_device_polling_transmit(spi_handle_, &t);

    // Swap double-buffer
    active_buf_ ^= 1;
  }

  void end_scanline_write() {
    // Nothing needed for polling mode
  }

  void fill_screen(uint16_t color) {
    begin_scanline_write();
    uint16_t line[DISPLAY_WIDTH];
    for (int i = 0; i < DISPLAY_WIDTH; i++) line[i] = color;
    for (int y = 0; y < DISPLAY_HEIGHT; y++) {
      push_scanline(line, DISPLAY_WIDTH);
    }
    end_scanline_write();
  }

 private:
  spi_device_handle_t spi_handle_ = nullptr;
  gpio_num_t dc_pin_;
  gpio_num_t cs_pin_;
  uint16_t line_buf_[2][DISPLAY_WIDTH];
  uint8_t active_buf_ = 0;

  void init_display() {
    // GC9A01 init sequence (from datasheet / common driver code)

    // Inter-register enable 1
    send_command(0xEF);

    // Inter-register enable 2
    send_command(0xEB);
    send_data_byte(0x14);

    send_command(0xFE);  // Inter-register enable 1
    send_command(0xEF);  // Inter-register enable 2

    send_command(0xEB);
    send_data_byte(0x14);

    send_command(0x84);
    send_data_byte(0x40);

    send_command(0x85);
    send_data_byte(0xFF);

    send_command(0x86);
    send_data_byte(0xFF);

    send_command(0x87);
    send_data_byte(0xFF);

    send_command(0x88);
    send_data_byte(0x0A);

    send_command(0x89);
    send_data_byte(0x21);

    send_command(0x8A);
    send_data_byte(0x00);

    send_command(0x8B);
    send_data_byte(0x80);

    send_command(0x8C);
    send_data_byte(0x01);

    send_command(0x8D);
    send_data_byte(0x01);

    send_command(0x8E);
    send_data_byte(0xFF);

    send_command(0x8F);
    send_data_byte(0xFF);

    // Display function control
    send_command(0xB6);
    {
      uint8_t data[] = {0x00, 0x00};
      send_data(data, 2);
    }

    // Memory access control - RGB color order
    send_command(0x36);
    send_data_byte(0x08);

    // Pixel format: 16-bit/pixel (RGB565)
    send_command(0x3A);
    send_data_byte(0x05);

    send_command(0x90);
    {
      uint8_t data[] = {0x08, 0x08, 0x08, 0x08};
      send_data(data, 4);
    }

    send_command(0xBD);
    send_data_byte(0x06);

    send_command(0xBC);
    send_data_byte(0x00);

    send_command(0xFF);
    {
      uint8_t data[] = {0x60, 0x01, 0x04};
      send_data(data, 3);
    }

    // Power control
    send_command(0xC3);
    send_data_byte(0x13);

    send_command(0xC4);
    send_data_byte(0x13);

    send_command(0xC9);
    send_data_byte(0x22);

    send_command(0xBE);
    send_data_byte(0x11);

    send_command(0xE1);
    {
      uint8_t data[] = {0x10, 0x0E};
      send_data(data, 2);
    }

    send_command(0xDF);
    {
      uint8_t data[] = {0x21, 0x0C, 0x02};
      send_data(data, 3);
    }

    // Gamma
    send_command(0xF0);
    {
      uint8_t data[] = {0x45, 0x09, 0x08, 0x08, 0x26, 0x2A};
      send_data(data, 6);
    }

    send_command(0xF1);
    {
      uint8_t data[] = {0x43, 0x70, 0x72, 0x36, 0x37, 0x6F};
      send_data(data, 6);
    }

    send_command(0xF2);
    {
      uint8_t data[] = {0x45, 0x09, 0x08, 0x08, 0x26, 0x2A};
      send_data(data, 6);
    }

    send_command(0xF3);
    {
      uint8_t data[] = {0x43, 0x70, 0x72, 0x36, 0x37, 0x6F};
      send_data(data, 6);
    }

    send_command(0xED);
    {
      uint8_t data[] = {0x1B, 0x0B};
      send_data(data, 2);
    }

    send_command(0xAE);
    send_data_byte(0x77);

    send_command(0xCD);
    send_data_byte(0x63);

    send_command(0x70);
    {
      uint8_t data[] = {0x07, 0x07, 0x04, 0x0E, 0x0F, 0x09, 0x07, 0x08, 0x03};
      send_data(data, 9);
    }

    send_command(0xE8);
    send_data_byte(0x34);

    send_command(0x62);
    {
      uint8_t data[] = {0x18, 0x0D, 0x71, 0xED, 0x70, 0x70, 0x18, 0x0F, 0x71, 0xEF, 0x70, 0x70};
      send_data(data, 12);
    }

    send_command(0x63);
    {
      uint8_t data[] = {0x18, 0x11, 0x71, 0xF1, 0x70, 0x70, 0x18, 0x13, 0x71, 0xF3, 0x70, 0x70};
      send_data(data, 12);
    }

    send_command(0x64);
    {
      uint8_t data[] = {0x28, 0x29, 0xF1, 0x01, 0xF1, 0x00, 0x07};
      send_data(data, 7);
    }

    send_command(0x66);
    {
      uint8_t data[] = {0x3C, 0x00, 0xCD, 0x67, 0x45, 0x45, 0x10, 0x00, 0x00, 0x00};
      send_data(data, 10);
    }

    send_command(0x67);
    {
      uint8_t data[] = {0x00, 0x3C, 0x00, 0x00, 0x00, 0x01, 0x54, 0x10, 0x32, 0x98};
      send_data(data, 10);
    }

    send_command(0x74);
    {
      uint8_t data[] = {0x10, 0x85, 0x80, 0x00, 0x00, 0x4E, 0x00};
      send_data(data, 7);
    }

    send_command(0x98);
    {
      uint8_t data[] = {0x3E, 0x07};
      send_data(data, 2);
    }

    // Tearing effect line on
    send_command(0x35);

    // Display inversion on
    send_command(0x21);

    // Sleep out
    send_command(0x11);
    vTaskDelay(pdMS_TO_TICKS(120));

    // Display on
    send_command(0x29);
    vTaskDelay(pdMS_TO_TICKS(20));
  }
};

}  // namespace cat_eyes
