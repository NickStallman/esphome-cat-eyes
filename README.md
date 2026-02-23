# ESPHome Cat Eyes

An ESPHome external component that drives two GC9A01 round LCD displays as animated cat eyes. Features 16 expressive animations, dynamic iris color, and autonomous idle behavior — all controllable via Home Assistant.

## Hardware

- **ESP32-WROOM-32** dev board (or equivalent)
- **2x GC9A01** 1.28" 240x240 round LCD displays
- Both displays share a single SPI bus (separate CS pins)

### Default Wiring

| Signal | GPIO | Notes |
|--------|------|-------|
| SPI CLK | 18 | Shared |
| SPI MOSI | 23 | Shared |
| CS Right | 5 | Right eye chip select |
| CS Left | 4 | Left eye chip select |
| DC | 22 | Data/Command (shared) |
| Reset | 21 | Hardware reset (shared) |

All pins are configurable in YAML.

## Installation

Add this to your ESPHome device YAML:

```yaml
esp32:
  board: esp32dev
  framework:
    type: esp-idf

external_components:
  - source: github://NickStallman/esphome-cat-eyes

cat_eyes:
  id: eyes
  clk_pin: GPIO18
  mosi_pin: GPIO23
  cs_right: GPIO5
  cs_left: GPIO4
  dc_pin: GPIO22
  reset_pin: GPIO21
```

> **Note:** This component requires the `esp-idf` framework (not Arduino) for direct SPI/DMA access.

## Configuration

```yaml
cat_eyes:
  id: eyes
  clk_pin: GPIO18          # SPI clock
  mosi_pin: GPIO23         # SPI data
  cs_right: GPIO5          # Right eye chip select
  cs_left: GPIO4           # Left eye chip select
  dc_pin: GPIO22           # Data/Command pin
  reset_pin: GPIO21        # Hardware reset pin
  spi_frequency: 80000000  # Optional, default 80MHz
```

## Home Assistant Services

Expose these services in your YAML to control the eyes from Home Assistant:

```yaml
api:
  services:
    - service: play_animation
      variables:
        name: string
      then:
        - lambda: 'id(eyes).play_animation(name);'

    - service: set_gaze
      variables:
        x: float
        y: float
      then:
        - lambda: 'id(eyes).set_gaze(x, y);'

    - service: set_palette
      variables:
        preset: string
      then:
        - lambda: 'id(eyes).set_palette(preset);'

    - service: set_palette_custom
      variables:
        iris_r: int
        iris_g: int
        iris_b: int
      then:
        - lambda: 'id(eyes).set_palette_rgb(iris_r, iris_g, iris_b);'

    - service: set_auto_idle
      variables:
        enabled: bool
      then:
        - lambda: 'id(eyes).set_auto_idle(enabled);'
```

### Animations

Pass any of these names to `play_animation`:

| Animation | Duration | Description |
|-----------|----------|-------------|
| `blink` | 0.3s | Quick blink |
| `slow_blink` | 1.6s | Drowsy slow blink |
| `double_blink` | 0.55s | Two quick blinks |
| `look_left` | 1.2s | Glance left |
| `look_right` | 1.2s | Glance right |
| `look_around` | 2.8s | Scan left, right, down |
| `angry` | 2.0s | Narrowed angry expression |
| `wide` | 1.85s | Wide-eyed alert |
| `surprised` | 1.3s | Startled, dilated pupils |
| `sleepy` | 3.5s | Drowsy, loops |
| `squint` | 1.1s | Suspicious squint |
| `wake_up` | 2.7s | Opening from sleep |
| `dilate` | 1.0s | Pupils expand |
| `constrict` | 0.8s | Pupils narrow to slits |
| `cute` | 5.3s | Puss-in-Boots pleading eyes |
| `idle` | 5.5s | Subtle micro-movements, loops |

### Color Presets

Pass to `set_palette`: `amber` (default), `green`, `blue`, `red`, `purple`

Or use `set_palette_custom` with any RGB value (0-255 per channel) to set a custom iris color. Mid-tones, edge, and ring colors are automatically derived.

### Gaze Control

`set_gaze` takes `x` and `y` from -1.0 to 1.0. This overrides the pupil position until an animation resets it.

### Auto Idle

When enabled (default), the eyes autonomously blink, glance around, and play subtle idle micro-movements. Disable with `set_auto_idle` to take full manual control.

## Architecture

- The component owns the SPI bus directly (not through ESPHome's `spi` component) for maximum rendering performance
- A dedicated FreeRTOS task on Core 1 handles continuous rendering while Core 0 handles WiFi/HA
- Scanline-based renderer pushes 240px lines via SPI at 80MHz
- Lid curves, pupil shape (cat slit to round), iris gradient, and specular highlights are computed per-pixel
- Animation system uses keyframe interpolation with 7 easing functions

## Full Example

See [example.yaml](example.yaml) for a complete working configuration.

## License

MIT
