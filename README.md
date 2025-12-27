# IKEA Head Lamp - Smart MQTT Firmware

Transform your IKEA head lamp into a smart RGB LED light with MQTT control, smooth animations, and persistent configuration.

## ✨ Features

- 🎨 **Full RGB Control** - 16.7 million colors via PWM
- 📡 **MQTT Integration** - Control via Home Assistant, Node-RED, or any MQTT client
- 🌅 **Sunrise Animation** - Gentle wake-up lighting with configurable duration
- 💾 **Persistent Storage** - Settings saved to NVS flash memory
- 🔘 **Physical Button** - Toggle power with hardware button
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
| `ikea_lamp/cmnd/power` | `on`, `off`, `toggle` | Control lamp power |
| `ikea_lamp/cmnd/brightness` | `0-100` | Set brightness (0-100%) |
| `ikea_lamp/cmnd/color` | `R,G,B` | Set color (e.g., `255,200,100`) |
| `ikea_lamp/cmnd/mode` | `static`, `animation` | Set operating mode |
| `ikea_lamp/cmnd/animation` | `sunrise` | Start animation |
| `ikea_lamp/cmnd/pause` | `true`, `false`, `toggle` | Pause/resume animation |
| `ikea_lamp/cmnd/apply_defaults` | any | Apply default settings |

### Configuration Topics

| Topic | Payload | Description |
|-------|---------|-------------|
| `ikea_lamp/config/default_brightness/set` | `0-100` | Default brightness |
| `ikea_lamp/config/default_color/set` | `R,G,B` | Default color |
| `ikea_lamp/config/sunrise_minutes/set` | `5-180` | Sunrise duration (minutes) |
| `ikea_lamp/config/min_pwm/set` | `0-100` | Min PWM duty cycle (%) |
| `ikea_lamp/config/max_pwm/set` | `0-100` | Max PWM duty cycle (%) |
| `ikea_lamp/config/save` | any | Save config to flash |
| `ikea_lamp/config/reset` | any | Reset to defaults |
| `ikea_lamp/config/request` | any | Request current config |

### State Topics

| Topic | Description |
|-------|-------------|
| `ikea_lamp/state/json` | Current state (power, brightness, color, animation) |
| `ikea_lamp/config/state` | Current configuration |

### Example Commands

```bash
# Turn on the lamp
mosquitto_pub -h 192.168.1.100 -t "ikea_lamp/cmnd/power" -m "on"

# Set to warm white at 70% brightness
mosquitto_pub -h 192.168.1.100 -t "ikea_lamp/cmnd/color" -m "255,200,160"
mosquitto_pub -h 192.168.1.100 -t "ikea_lamp/cmnd/brightness" -m "70"

# Start 30-minute sunrise animation
mosquitto_pub -h 192.168.1.100 -t "ikea_lamp/config/sunrise_minutes/set" -m "30"
mosquitto_pub -h 192.168.1.100 -t "ikea_lamp/config/save" -m "1"
mosquitto_pub -h 192.168.1.100 -t "ikea_lamp/cmnd/animation" -m "sunrise"
```

## 🏠 Home Assistant Integration

Add to your `configuration.yaml`:

```yaml
mqtt:
  light:
    - name: "IKEA Lamp"
      state_topic: "ikea_lamp/state/json"
      command_topic: "ikea_lamp/cmnd/power"
      brightness_state_topic: "ikea_lamp/state/json"
      brightness_command_topic: "ikea_lamp/cmnd/brightness"
      rgb_state_topic: "ikea_lamp/state/json"
      rgb_command_topic: "ikea_lamp/cmnd/color"
      brightness_scale: 100
      payload_on: "on"
      payload_off: "off"
      value_template: "{{ value_json.power }}"
      brightness_value_template: "{{ value_json.brightness }}"
      rgb_value_template: "{{ value_json.color[0] }},{{ value_json.color[1] }},{{ value_json.color[2] }}"
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
mosquitto_pub -t "ikea_lamp/config/min_pwm/set" -m "20"

# Set maximum PWM to 100% (LEDs at full power at brightness=100)
mosquitto_pub -t "ikea_lamp/config/max_pwm/set" -m "100"

# Save configuration
mosquitto_pub -t "ikea_lamp/config/save" -m "1"
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
- Pause/resume: `mosquitto_pub -t "ikea_lamp/cmnd/pause" -m "toggle"`
- Check WiFi connection (animation updates in loop)
- Restart animation: publish to `ikea_lamp/cmnd/animation`

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
