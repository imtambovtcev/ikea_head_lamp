# IKEA Head Lamp - Smart MQTT Firmware

Transform your IKEA head lamp into a smart RGB LED light with MQTT control, smooth animations, and persistent configuration.

## ✨ Features

- 🎨 **Full RGB Control** - 16.7 million colors via PWM
- 📡 **MQTT Integration** - Control via Home Assistant, Node-RED, or any MQTT client
- 🌅 **6 Animations** - Sunrise, Sunset, Rainbow, Fire, Breathe, Ocean
- 💾 **Persistent Storage** - Settings saved to NVS flash memory
- 🔘 **Physical Button** - Single click (power), long press (pause/play), double-click (rainbow)
- 🔧 **Configurable PWM** - Calibrate brightness range for your LED hardware
- 🏗️ **Modular Architecture** - Clean, maintainable, extensible code

## 🛠️ Hardware Requirements

- **ESP32-C3-DevKitM-1** (or compatible ESP32-C3 board)
- **RGB LED** (from IKEA lamp or similar)
- **Push button** (connected to GPIO5)
- **Resistors** for LED current limiting (if needed)

### Pin Configuration

| Component | GPIO Pin |
|-----------|----------|
| Red LED   | 1        |
| Green LED | 4        |
| Blue LED  | 3        |
| Button    | 5 (with internal pull-up) |

### Button Controls

| Action | Function |
|--------|----------|
| **Single Click** | Toggle power (turn on to default color OR turn off) |
| **Long Press** | Pause/play current animation |
| **Double Click** | Start rainbow animation |

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

## 🎮 MQTT Control

### Command Topics

| Topic | Payload | Description |
|-------|---------|-------------|
| `ikea_head_lamp/cmnd/power` | `on`, `off`, `toggle` | Control lamp power |
| `ikea_head_lamp/cmnd/brightness` | `0-100` | Set brightness (0-100%) |
| `ikea_head_lamp/cmnd/color` | `R,G,B` | Set color (e.g., `255,200,100`) |
| `ikea_head_lamp/cmnd/mode` | `static`, `animation` | Set operating mode |
| `ikea_head_lamp/cmnd/animation` | `sunrise`, `sunset`, `rainbow`, `fire`, `breathe`, `ocean`, `stop` | Start/stop animation (see examples below) |
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
| `ikea_head_lamp/config/save` | any | Save config to flash |
| `ikea_head_lamp/config/reset` | any | Reset to defaults |
| `ikea_head_lamp/config/request` | any | Request current config |

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

See [REFACTORING.md](REFACTORING.md) for detailed architecture documentation.

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
├── include/           Configuration headers (WiFi, MQTT)
├── lib/               External libraries
├── src/               Source code
│   ├── hw/           Hardware layer
│   ├── state/        State management
│   ├── net/          Network layer
│   ├── anim/         Animation system
│   └── main.cpp      Main application
├── platformio.ini     Build configuration
└── README.md         This file
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

---

**Built with ❤️ using clean architecture principles**

For detailed architecture information, see [REFACTORING.md](REFACTORING.md)  
For quick API reference, see [QUICK_REFERENCE.md](QUICK_REFERENCE.md)
