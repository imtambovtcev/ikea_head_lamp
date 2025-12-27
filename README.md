![IKEA Head Lamp Logo](docs/logo.png)

# IKEA ISKÄRNA Head Lamp - Smart MQTT Firmware

Transform your IKEA ISKÄRNA head lamp into a smart RGB LED light with MQTT control, smooth animations, and persistent configuration.

## 📚 Documentation

- **[Hardware Modification Guide](docs/HARDWARE_MOD.md)** - Complete guide for replacing the WDP80 chip with ESP32-C3
- **[Board Images](docs/)** - Reference images for PCB contacts and soldering

## ✨ Features

### Core Functionality
- 🎨 **Full RGB Control** - 16.7 million colors via 13-bit PWM with gamma correction (2.2)
- 📡 **MQTT Integration** - Complete control via Home Assistant, Node-RED, or any MQTT client
- 🌅 **6 Animations** - Sunrise, Sunset, Rainbow, Fire, Breathe, Ocean with customizable parameters
- 💾 **Persistent Configuration** - All settings saved to NVS flash memory, survive reboots
- 🔘 **Physical Button Control** - Single click (power toggle), long press (pause/play), double-click (favorite animation)
- ⭐ **Favorite Animation** - Save your preferred animation with custom parameters for instant access
- 🔧 **Hardware Calibration** - Configurable PWM range (20-100%) to match your LED characteristics

### Advanced Features
- 🎯 **Animation Parameters** - Fine-tune duration, brightness, speed, colors for each animation
- ⏸️ **Pause/Resume** - Pause any running animation and resume from the same state
- 📊 **Real-time Monitoring** - MQTT state updates, heartbeat, diagnostics (heap, WiFi RSSI, loop rate)
- 🏗️ **Modular Architecture** - Clean, maintainable, extensible codebase with hardware abstraction
- ✅ **Comprehensive Testing** - Python-based MQTT test suite with 50+ automated tests
- 🔒 **Safe Design** - Watchdog timer, WiFi reconnection, MQTT auto-reconnect, bounded animations

## 🛠️ Hardware Requirements

- **ESP32-C3 Super Mini** (tested) or **ESP32-C3-DevKitM-1** (compatible ESP32-C3 board)
- **RGB LED** (from IKEA lamp)
- **Push button** (connected to GPIO5 via 2kΩ resistor)
- **24V to 5V DC-DC converter** (buck converter)
- **2kΩ resistors** × 4 (mandatory for safety)
- **Optional:** 470µF capacitor (power stability), 200nF capacitor (button debouncing)

> **📖 See [Hardware Modification Guide](docs/HARDWARE_MOD.md) for complete modification instructions with images**

### Pin Configuration

| Component | ESP32 GPIO |
|-----------|------------|
| Red LED   | 1 (via 2kΩ resistor) |
| Green LED | 4 (via 2kΩ resistor) |
| Blue LED  | 3 (via 2kΩ resistor) |
| Button    | 5 (via 2kΩ resistor, internal pull-up enabled) |

> **⚠️ Important:** All GPIO connections must use 2kΩ resistors for safety!

### Button Controls

| Action | Function |
|--------|----------|
| **Single Click** | Toggle power (turn on to default color OR turn off) |
| **Long Press** | Pause/play current animation |
| **Double Click** | Start favorite animation (configurable via MQTT) |

## 📦 Installation

### Prerequisites

- [PlatformIO](https://platformio.org/) installed
- USB cable for ESP32-C3
- MQTT broker (e.g., Mosquitto, Home Assistant)

### Setup Steps

1. **Clone and configure:**
   ```bash
   cd ikea_head_lamp
   ```

2. **Configure WiFi** - Edit `include/wifi_config.h`:
   ```cpp
   #define WIFI_SSID     "YourNetworkName"
   #define WIFI_PASSWORD "YourPassword"
   ```

3. **Configure MQTT** - Edit `include/mqtt_config.h`:
   ```cpp
   #define MQTT_HOST     "192.168.1.100"  // Your MQTT broker IP
   #define MQTT_PORT     1883
   #define MQTT_USER     ""               // Optional
   #define MQTT_PASSWORD ""               // Optional
   ```

4. **Build and upload:**
   ```bash
   pio run -t upload
   ```

5. **Monitor serial output:**
   ```bash
   pio device monitor
   ```

## 🧪 Testing

The project includes a comprehensive Python-based MQTT test suite with 50+ automated tests covering all features.

### Prerequisites

```bash
cd test
pip install -r requirements.txt
```

### Configuration

Copy the example config and edit with your broker details:
```bash
cp mqtt_test_config.py.example mqtt_test_config.py
# Edit mqtt_test_config.py with your MQTT broker IP and device topic
```

### Running Tests

```bash
# Run all automated tests
python3 run_all_tests.py

# Run individual test suites
python3 test_basic_commands.py      # Power, brightness, color
python3 test_configuration.py       # Config management, NVS persistence
python3 test_animations.py          # All 6 animations with parameters
python3 test_pause_play.py          # Animation pause/resume
python3 test_favorite_animation.py  # Favorite animation feature
python3 test_button_controls.py     # Manual button testing (interactive)
```

### Test Coverage

The test suite validates:
- ✅ Basic commands (power, brightness, color, defaults)
- ✅ Configuration management (CRUD operations, NVS persistence)
- ✅ All animations with various parameter combinations
- ✅ Animation pause/play functionality
- ✅ Favorite animation configuration and triggering
- ✅ Button controls (requires manual interaction)
- ✅ State synchronization and MQTT message validation

See [test/README](test/README) for detailed test documentation.

## 🎮 MQTT Control

### Command Topics

| Topic | Payload | Description |
|-------|---------|-------------|
| `ikea_head_lamp/cmnd/power` | `on`, `off`, `toggle` | Control lamp power |
| `ikea_head_lamp/cmnd/brightness` | `0-100` | Set brightness (0-100%) |
| `ikea_head_lamp/cmnd/color` | `R,G,B` | Set color (e.g., `255,200,100`) |
| `ikea_head_lamp/cmnd/mode` | `static`, `animation` | Set operating mode |
| `ikea_head_lamp/cmnd/animation` | `sunrise`, `sunset`, `rainbow`, `fire`, `breathe`, `ocean`, `favorite`, `stop` | Start/stop animation (see examples below) |
| `ikea_head_lamp/cmnd/pause` | `true`, `false`, `toggle` | Pause/resume animation |
| `ikea_head_lamp/cmnd/query` | any | Request immediate state publish |
| `ikea_head_lamp/cmnd/test` | `color`, `rgb` | Run RGB color test (R→G→B cycle) |
| `ikea_head_lamp/cmnd/apply_defaults` | any | Apply default settings |

### Configuration Topics

| Topic | Payload | Description |
|-------|---------|-------------|
| `ikea_head_lamp/config/default_brightness/set` | `0-100` | Default brightness |
| `ikea_head_lamp/config/default_color/set` | `R,G,B` | Default color |
| `ikea_head_lamp/config/sunrise_minutes/set` | `5-180` | Sunrise duration (minutes) |
| `ikea_head_lamp/config/min_pwm/set` | `0-100` | Min PWM duty cycle (%) |
| `ikea_head_lamp/config/max_pwm/set` | `0-100` | Max PWM duty cycle (%) |
| `ikea_head_lamp/config/favorite_animation/set` | animation spec | Set favorite animation for double-click button |
| `ikea_head_lamp/config/save` | any | Save config to flash |
| `ikea_head_lamp/config/reset` | any | Reset to defaults |
| `ikea_head_lamp/config/request` | any | Request current config |

**Favorite Animation Configuration**

The favorite animation is triggered by double-clicking the button. You can configure which animation and parameters to use:

```bash
# Set favorite to fire with intensity=80, speed=7
mosquitto_pub -h 192.168.1.100 -t "ikea_head_lamp/config/favorite_animation/set" -m "fire:intensity=80,speed=7"

# Set favorite to breathe with 6-second cycle, blue color
mosquitto_pub -h 192.168.1.100 -t "ikea_head_lamp/config/favorite_animation/set" -m "breathe:duration=6,color=0,100,255"

# Set favorite to ocean waves
mosquitto_pub -h 192.168.1.100 -t "ikea_head_lamp/config/favorite_animation/set" -m "ocean:speed=8,brightness=50"

# Set favorite to sunrise (10-minute warm sunrise)
mosquitto_pub -h 192.168.1.100 -t "ikea_head_lamp/config/favorite_animation/set" -m "sunrise:duration=10,brightness=80"

# Set favorite to just rainbow (no parameters)
mosquitto_pub -h 192.168.1.100 -t "ikea_head_lamp/config/favorite_animation/set" -m "rainbow"

# Trigger the favorite animation via MQTT
mosquitto_pub -h 192.168.1.100 -t "ikea_head_lamp/cmnd/animation" -m "favorite"

# Save the configuration to flash (persists after reboot)
mosquitto_pub -h 192.168.1.100 -t "ikea_head_lamp/config/save" -m "1"
```

The default favorite animation is **fire** with intensity=70, speed=5. This can be customized to any animation with your preferred parameters and will be triggered by:
- Double-clicking the physical button
- Sending `favorite` to the animation command topic

### State Topics

| Topic | Description |
|-------|-------------|
| `ikea_head_lamp/state/json` | Current state (JSON: power, brightness, color, animation, progress) |
| `ikea_head_lamp/config/state` | Current configuration (JSON) |
| `ikea_head_lamp/heartbeat` | Uptime in seconds (published every 10s) |
| `ikea_head_lamp/diagnostics` | System diagnostics (heap, WiFi RSSI, loop rate) |

### Example Commands

```bash
# Turn on the lamp
mosquitto_pub -h 192.168.1.100 -t "ikea_head_lamp/cmnd/power" -m "on"

# Set to warm white at 70% brightness
mosquitto_pub -h 192.168.1.100 -t "ikea_head_lamp/cmnd/color" -m "255,147,41"
mosquitto_pub -h 192.168.1.100 -t "ikea_head_lamp/cmnd/brightness" -m "70"

# Simple sunrise (use config defaults: 30 min, warm white)
mosquitto_pub -h 192.168.1.100 -t "ikea_head_lamp/cmnd/animation" -m "sunrise"

# 1-minute blue sunrise
mosquitto_pub -h 192.168.1.100 -t "ikea_head_lamp/cmnd/animation" -m "sunrise:duration=1,color=0,100,255"

# 5-minute dim sunrise to 50% brightness
mosquitto_pub -h 192.168.1.100 -t "ikea_head_lamp/cmnd/animation" -m "sunrise:duration=5,brightness=50"

# 10-minute warm orange sunrise
mosquitto_pub -h 192.168.1.100 -t "ikea_head_lamp/cmnd/animation" -m "sunrise:duration=10,brightness=80,color=255,100,0"

# Start rainbow animation
mosquitto_pub -h 192.168.1.100 -t "ikea_head_lamp/cmnd/animation" -m "rainbow"

# Start fire animation (default intensity and speed)
mosquitto_pub -h 192.168.1.100 -t "ikea_head_lamp/cmnd/animation" -m "fire"

# Fire with high intensity and fast flicker
mosquitto_pub -h 192.168.1.100 -t "ikea_head_lamp/cmnd/animation" -m "fire:intensity=90,speed=8"

# Start breathe animation (4 second cycle, current color)
mosquitto_pub -h 192.168.1.100 -t "ikea_head_lamp/cmnd/animation" -m "breathe"

# Breathe with custom parameters (6 sec cycle, blue color, 20-80% brightness range)
mosquitto_pub -h 192.168.1.100 -t "ikea_head_lamp/cmnd/animation" -m "breathe:duration=6,brightness=80,min_brightness=20,color=0,100,255"

# Start sunset (30 min, dims to off)
mosquitto_pub -h 192.168.1.100 -t "ikea_head_lamp/cmnd/animation" -m "sunset"

# Fast 10-minute sunset dimming to 10% (stays on)
mosquitto_pub -h 192.168.1.100 -t "ikea_head_lamp/cmnd/animation" -m "sunset:duration=10,brightness=10"

# Start ocean waves
mosquitto_pub -h 192.168.1.100 -t "ikea_head_lamp/cmnd/animation" -m "ocean"

# Fast ocean waves at 50% brightness
mosquitto_pub -h 192.168.1.100 -t "ikea_head_lamp/cmnd/animation" -m "ocean:speed=8,brightness=50"

# Run RGB color test
mosquitto_pub -h 192.168.1.100 -t "ikea_head_lamp/cmnd/test" -m "color"

# Query current state
mosquitto_pub -h 192.168.1.100 -t "ikea_head_lamp/cmnd/query" -m "1"
```

### Animation Parameters

**Sunrise animation** supports these parameters (all optional):
- `duration=X` - Duration in minutes (default: 30)
- `brightness=X` - Final brightness 0-100 (default: 100)
- `color=R,G,B` - Final color (default: 255,147,41 warm white)

Sunrise automatically progresses through color temperatures: **Deep Red (2000K) → Orange → Yellow → Final Color** over the first 70% of the animation, then holds the final color.

**Sunset animation** supports these parameters (all optional):
- `duration=X` - Duration in minutes (default: 30)
- `brightness=X` - Final brightness 0-100 (0 = turn off, default: 0)

Sunset reverses the sunrise: current color → warm white → orange → red → final brightness (or off).

**Fire animation** supports these parameters (all optional):
- `intensity=X` - Flicker intensity 0-100 (default: 70, higher = more wild flickering)
- `speed=X` - Flicker speed 1-10 (default: 5, higher = faster changes)

Fire creates realistic flame effects with warm colors (red → orange → yellow) and random intensity variations.

**Breathe animation** supports these parameters (all optional):
- `duration=X` - Seconds per breath cycle (default: 4)
- `brightness=X` - Max brightness 0-100 (default: 70)
- `min_brightness=X` - Min brightness 0-100 (default: 10)
- `color=R,G,B` - Color to breathe (default: current color)

Breathe creates smooth fade in/out at specified color, perfect for meditation or relaxation.

**Ocean animation** supports these parameters (all optional):
- `speed=X` - Wave speed 1-10 (default: 5, higher = faster waves)
- `brightness=X` - Max brightness 0-100 (default: 70)

Ocean creates gentle waves through blue-cyan-teal spectrum with calming transitions.

## 🏠 Home Assistant Integration

### MQTT Light Entity

Add to your `configuration.yaml`:

```yaml
mqtt:
  light:
    - name: "IKEA Lamp"
      unique_id: ikea_head_lamp
      state_topic: "ikea_head_lamp/state/json"
      command_topic: "ikea_head_lamp/cmnd/power"
      brightness_state_topic: "ikea_head_lamp/state/json"
      brightness_command_topic: "ikea_head_lamp/cmnd/brightness"
      rgb_state_topic: "ikea_head_lamp/state/json"
      rgb_command_topic: "ikea_head_lamp/cmnd/color"
      brightness_scale: 100
      payload_on: "on"
      payload_off: "off"
      value_template: "{{ value_json.pwr }}"
      brightness_value_template: "{{ value_json.bri }}"
      rgb_value_template: "{{ value_json.rgb[0] }},{{ value_json.rgb[1] }},{{ value_json.rgb[2] }}"
```

### Automation Examples

**Gentle wake-up (30-minute sunrise at 7 AM):**
```yaml
automation:
  - alias: "Morning Wake Up"
    trigger:
      platform: time
      at: "07:00:00"
    action:
      - service: mqtt.publish
        data:
          topic: "ikea_head_lamp/cmnd/animation"
          payload: "sunrise:duration=30,brightness=100"
```

**Evening reading light (warm white):**
```yaml
automation:
  - alias: "Evening Reading"
    trigger:
      platform: time
      at: "20:00:00"
    action:
      - service: mqtt.publish
        data:
          topic: "ikea_head_lamp/cmnd/color"
          payload: "255,147,41"
      - service: mqtt.publish
        data:
          topic: "ikea_head_lamp/cmnd/brightness"
          payload: "60"
```

## 🏗️ Architecture

The firmware uses a clean modular architecture:

```
├── hw/              Hardware abstraction (LEDs, button)
├── state/           State management (runtime + persistent)
├── net/             Network layer (WiFi, MQTT)
├── anim/            Animation system (sunrise, etc.)
└── main.cpp         Orchestration layer
```

**Benefits:**
- ✅ Each module has one responsibility
- ✅ Easy to test and debug
- ✅ Simple to add new features
- ✅ Hardware-agnostic animation logic

## 🔧 Customization

### Adjusting Brightness Mapping

LEDs often have a minimum PWM duty cycle below which they don't light. Configure this via MQTT:

```bash
# Set minimum PWM to 20% (LEDs barely visible at brightness=1)
mosquitto_pub -t "ikea_head_lamp/config/min_pwm/set" -m "20"

# Set maximum PWM to 100% (LEDs at full power at brightness=100)
mosquitto_pub -t "ikea_head_lamp/config/max_pwm/set" -m "100"

# Save configuration
mosquitto_pub -t "ikea_head_lamp/config/save" -m "1"
```

### Adding New Animations

1. Create `src/anim/YourAnimation.h` and `.cpp`
2. Implement the animation interface
3. Add to `AnimationEngine`
4. Wire MQTT command in `main.cpp`

Example structure:
```cpp
class YourAnimation {
  void start(DeviceState*, DeviceConfig*);
  void stop(DeviceState*);
  bool update(DeviceState*, DeviceConfig*);
  bool isActive() const;
};
```

## 🐛 Troubleshooting

### Lamp doesn't connect to WiFi
- Verify credentials in `include/wifi_config.h`
- Check WiFi signal strength
- Monitor serial output for connection status

### MQTT not working
- Verify broker IP and credentials
- Check firewall rules
- Test broker with: `mosquitto_sub -h IP -t "#" -v`

### LEDs too dim or too bright
- Adjust PWM range: `min_pwm` and `max_pwm`
- Check hardware current limiting resistors
- Verify GPIO pin connections

### Button not responding
- Check GPIO5 connection and pull-up resistor (2kΩ to VCC recommended)
- Internal pull-up is enabled in firmware
- Monitor serial output for button events

### Animation stuck or jerky
- Pause/resume: `mosquitto_pub -t "ikea_head_lamp/cmnd/pause" -m "toggle"`
- Check WiFi connection (animation updates in loop)
- Restart animation: publish to `ikea_head_lamp/cmnd/animation`

## 📊 Memory Usage

| Resource | Used | Total | Available |
|----------|------|-------|-----------|
| Flash    | ~732 KB | 1280 KB | 548 KB (42%) |
| RAM      | ~38 KB | 320 KB | 282 KB (88%) |

Plenty of room for additional features!

## 🚀 Development

### Building

```bash
# Clean build
pio run -t clean
pio run

# Upload
pio run -t upload

# Monitor serial
pio device monitor -b 115200
```

### Project Structure

```
ikea_head_lamp/
├── docs/              Documentation and hardware images
│   ├── HARDWARE_MOD.md  Hardware modification guide
│   ├── front.png        PCB front (unmodified)
│   ├── back.png         PCB back (unmodified)
│   ├── front_s.png      PCB front (wired)
│   └── back_s.png       PCB back (wired)
├── include/           Configuration headers (WiFi, MQTT)
├── lib/               External libraries
├── src/               Source code
│   ├── hw/           Hardware abstraction layer
│   ├── state/        State management & configuration
│   ├── net/          Network layer (WiFi, MQTT)
│   ├── anim/         Animation system (6 animations)
│   └── main.cpp      Main application loop
├── test/              Python MQTT test suite
│   ├── mqtt_test_utils.py    Test framework
│   ├── run_all_tests.py      Master test runner
│   └── test_*.py             Individual test suites
├── platformio.ini     PlatformIO build configuration
└── README.md          This file
```

## 📝 License

This project is provided as-is for personal and educational use.

## 🙏 Credits

- **Hardware:** IKEA head lamp (modified)
- **Platform:** ESP32-C3 by Espressif
- **Framework:** Arduino ESP32
- **Libraries:** PubSubClient (MQTT), Preferences (NVS)

## 🔗 Resources

- [PlatformIO Documentation](https://docs.platformio.org/)
- [ESP32-C3 Datasheet](https://www.espressif.com/sites/default/files/documentation/esp32-c3_datasheet_en.pdf)
- [MQTT Protocol](https://mqtt.org/)
- [Home Assistant MQTT Integration](https://www.home-assistant.io/integrations/mqtt/)
