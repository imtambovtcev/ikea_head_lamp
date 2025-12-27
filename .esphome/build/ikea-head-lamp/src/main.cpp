// Auto generated code by esphome
// ========== AUTO GENERATED INCLUDE BLOCK BEGIN ===========
#include "esphome.h"
using namespace esphome;
using std::isnan;
using std::min;
using std::max;
using namespace time;
using namespace light;
using namespace number;
using namespace text_sensor;
using namespace sensor;
using namespace binary_sensor;
using namespace button;
logger::Logger *logger_logger_id;
wifi::WiFiComponent *wifi_wificomponent_id;
mdns::MDNSComponent *mdns_mdnscomponent_id;
esphome::ESPHomeOTAComponent *esphome_esphomeotacomponent_id;
safe_mode::SafeModeComponent *safe_mode_safemodecomponent_id;
api::APIServer *api_apiserver_id;
using namespace api;
preferences::IntervalSyncer *preferences_intervalsyncer_id;
homeassistant::HomeassistantTime *esptime;
using namespace output;
esp32::ESP32InternalGPIOPin *esp32_esp32internalgpiopin_id;
ledc::LEDCOutput *out_red;
esp32::ESP32InternalGPIOPin *esp32_esp32internalgpiopin_id_2;
ledc::LEDCOutput *out_green;
esp32::ESP32InternalGPIOPin *esp32_esp32internalgpiopin_id_3;
ledc::LEDCOutput *out_blue;
rgb::RGBLightOutput *rgb_rgblightoutput_id;
light::LightState *ikea_lamp;
template_::TemplateNumber *sunrise_minutes;
template_::TemplateTextSensor *last_event;
template_::TemplateSensor *last_event_time;
gpio::GPIOBinarySensor *lamp_button;
binary_sensor::DelayedOnFilter *binary_sensor_delayedonfilter_id;
binary_sensor::DelayedOffFilter *binary_sensor_delayedofffilter_id;
binary_sensor::PressTrigger *binary_sensor_presstrigger_id;
Automation<> *automation_id;
text_sensor::TextSensorPublishAction<> *text_sensor_textsensorpublishaction_id;
sensor::SensorPublishAction<> *sensor_sensorpublishaction_id;
light::LightIsOnCondition<> *light_lightisoncondition_id;
IfAction<> *ifaction_id;
light::LightControlAction<> *light_lightcontrolaction_id;
script::SingleScript<> *turn_on_fixed_light;
script::RestartScript<> *run_sunrise;
script::SingleScript<> *stop_sunrise;
Automation<> *automation_id_2;
light::LightControlAction<> *light_lightcontrolaction_id_2;
Automation<> *automation_id_3;
template_::TemplateButton *template__templatebutton_id;
button::ButtonPressTrigger *button_buttonpresstrigger_id;
Automation<> *automation_id_5;
script::ScriptExecuteAction<script::Script<>> *script_scriptexecuteaction_id_2;
template_::TemplateButton *template__templatebutton_id_2;
button::ButtonPressTrigger *button_buttonpresstrigger_id_2;
Automation<> *automation_id_6;
script::ScriptExecuteAction<script::Script<>> *script_scriptexecuteaction_id_3;
script::ScriptExecuteAction<script::Script<>> *script_scriptexecuteaction_id;
esp32::ESP32InternalGPIOPin *esp32_esp32internalgpiopin_id_4;
globals::GlobalsComponent<bool> *sunrise_running;
globals::GlobalsComponent<float> *sunrise_level;
LambdaAction<> *lambdaaction_id;
light::LightControlAction<> *light_lightcontrolaction_id_3;
LambdaCondition<> *lambdacondition_id;
WhileAction<> *whileaction_id;
LambdaAction<> *lambdaaction_id_2;
light::LightControlAction<> *light_lightcontrolaction_id_4;
DelayAction<> *delayaction_id;
Automation<> *automation_id_4;
LambdaAction<> *lambdaaction_id_3;
#define yield() esphome::yield()
#define millis() esphome::millis()
#define micros() esphome::micros()
#define delay(x) esphome::delay(x)
#define delayMicroseconds(x) esphome::delayMicroseconds(x)
// ========== AUTO GENERATED INCLUDE BLOCK END ==========="

void setup() {
  // ========== AUTO GENERATED CODE BEGIN ===========
  App.reserve_text_sensor(1);
  App.reserve_sensor(1);
  App.reserve_number(1);
  App.reserve_light(1);
  App.reserve_button(2);
  App.reserve_binary_sensor(1);
  // network:
  //   enable_ipv6: false
  //   min_ipv6_addr_count: 0
  // esphome:
  //   name: ikea-head-lamp
  //   friendly_name: IKEA Head Lamp
  //   min_version: 2025.7.5
  //   build_path: build/ikea-head-lamp
  //   platformio_options: {}
  //   includes: []
  //   libraries: []
  //   name_add_mac_suffix: false
  //   debug_scheduler: false
  //   areas: []
  //   devices: []
  App.pre_setup("ikea-head-lamp", "IKEA Head Lamp", "", __DATE__ ", " __TIME__, false);
  App.reserve_components(21);
  // time:
  // light:
  // number:
  // text_sensor:
  // sensor:
  // binary_sensor:
  // button:
  // logger:
  //   id: logger_logger_id
  //   baud_rate: 115200
  //   tx_buffer_size: 512
  //   deassert_rts_dtr: false
  //   task_log_buffer_size: 768
  //   hardware_uart: USB_CDC
  //   level: DEBUG
  //   logs: {}
  logger_logger_id = new logger::Logger(115200, 512);
  logger_logger_id->create_pthread_key();
  logger_logger_id->init_log_buffer(768);
  logger_logger_id->set_log_level(ESPHOME_LOG_LEVEL_DEBUG);
  logger_logger_id->set_uart_selection(logger::UART_SELECTION_USB_CDC);
  logger_logger_id->pre_setup();
  logger_logger_id->set_component_source("logger");
  App.register_component(logger_logger_id);
  // wifi:
  //   id: wifi_wificomponent_id
  //   domain: .local
  //   reboot_timeout: 15min
  //   power_save_mode: LIGHT
  //   fast_connect: false
  //   passive_scan: false
  //   enable_on_boot: true
  //   networks:
  //     - ssid: ASUS_88
  //       password: vano24810
  //       id: wifi_wifiap_id
  //       priority: 0.0
  //   use_address: ikea-head-lamp.local
  wifi_wificomponent_id = new wifi::WiFiComponent();
  wifi_wificomponent_id->set_use_address("ikea-head-lamp.local");
  {
  wifi::WiFiAP wifi_wifiap_id = wifi::WiFiAP();
  wifi_wifiap_id.set_ssid("ASUS_88");
  wifi_wifiap_id.set_password("vano24810");
  wifi_wifiap_id.set_priority(0.0f);
  wifi_wificomponent_id->add_sta(wifi_wifiap_id);
  }
  wifi_wificomponent_id->set_reboot_timeout(900000);
  wifi_wificomponent_id->set_power_save_mode(wifi::WIFI_POWER_SAVE_LIGHT);
  wifi_wificomponent_id->set_fast_connect(false);
  wifi_wificomponent_id->set_passive_scan(false);
  wifi_wificomponent_id->set_enable_on_boot(true);
  wifi_wificomponent_id->set_component_source("wifi");
  App.register_component(wifi_wificomponent_id);
  // mdns:
  //   id: mdns_mdnscomponent_id
  //   disabled: false
  //   services: []
  mdns_mdnscomponent_id = new mdns::MDNSComponent();
  mdns_mdnscomponent_id->set_component_source("mdns");
  App.register_component(mdns_mdnscomponent_id);
  // ota:
  // ota.esphome:
  //   platform: esphome
  //   id: esphome_esphomeotacomponent_id
  //   version: 2
  //   port: 3232
  esphome_esphomeotacomponent_id = new esphome::ESPHomeOTAComponent();
  esphome_esphomeotacomponent_id->set_port(3232);
  esphome_esphomeotacomponent_id->set_component_source("esphome.ota");
  App.register_component(esphome_esphomeotacomponent_id);
  // safe_mode:
  //   id: safe_mode_safemodecomponent_id
  //   boot_is_good_after: 1min
  //   disabled: false
  //   num_attempts: 10
  //   reboot_timeout: 5min
  safe_mode_safemodecomponent_id = new safe_mode::SafeModeComponent();
  safe_mode_safemodecomponent_id->set_component_source("safe_mode");
  App.register_component(safe_mode_safemodecomponent_id);
  if (safe_mode_safemodecomponent_id->should_enter_safe_mode(10, 300000, 60000)) return;
  // api:
  //   id: api_apiserver_id
  //   port: 6053
  //   password: ''
  //   reboot_timeout: 15min
  //   batch_delay: 100ms
  //   custom_services: false
  api_apiserver_id = new api::APIServer();
  api_apiserver_id->set_component_source("api");
  App.register_component(api_apiserver_id);
  api_apiserver_id->set_port(6053);
  api_apiserver_id->set_reboot_timeout(900000);
  api_apiserver_id->set_batch_delay(100);
  // esp32:
  //   board: esp32-c3-devkitm-1
  //   variant: ESP32C3
  //   flash_size: 4MB
  //   framework:
  //     version: 3.1.3
  //     advanced:
  //       ignore_efuse_custom_mac: false
  //     source: pioarduino/framework-arduinoespressif32@https:github.com/espressif/arduino-esp32/releases/download/3.1.3/esp32-3.1.3.zip
  //     platform_version: https:github.com/pioarduino/platform-espressif32/releases/download/53.03.13/platform-espressif32.zip
  //     type: arduino
  //   cpu_frequency: 160MHZ
  setCpuFrequencyMhz(160);
  // preferences:
  //   id: preferences_intervalsyncer_id
  //   flash_write_interval: 60s
  preferences_intervalsyncer_id = new preferences::IntervalSyncer();
  preferences_intervalsyncer_id->set_write_interval(60000);
  preferences_intervalsyncer_id->set_component_source("preferences");
  App.register_component(preferences_intervalsyncer_id);
  // time.homeassistant:
  //   platform: homeassistant
  //   id: esptime
  //   timezone: GMT0
  //   update_interval: 15min
  esptime = new homeassistant::HomeassistantTime();
  esptime->set_timezone("GMT0");
  esptime->set_update_interval(900000);
  esptime->set_component_source("homeassistant.time");
  App.register_component(esptime);
  // output:
  // output.ledc:
  //   platform: ledc
  //   pin:
  //     number: 1
  //     mode:
  //       output: true
  //       input: false
  //       open_drain: false
  //       pullup: false
  //       pulldown: false
  //     id: esp32_esp32internalgpiopin_id
  //     inverted: false
  //     ignore_pin_validation_error: false
  //     ignore_strapping_warning: false
  //     drive_strength: 20.0
  //   id: out_red
  //   frequency: 5000.0
  //   zero_means_zero: false
  esp32_esp32internalgpiopin_id = new esp32::ESP32InternalGPIOPin();
  esp32_esp32internalgpiopin_id->set_pin(::GPIO_NUM_1);
  esp32_esp32internalgpiopin_id->set_inverted(false);
  esp32_esp32internalgpiopin_id->set_drive_strength(::GPIO_DRIVE_CAP_2);
  esp32_esp32internalgpiopin_id->set_flags(gpio::Flags::FLAG_OUTPUT);
  out_red = new ledc::LEDCOutput(esp32_esp32internalgpiopin_id);
  out_red->set_component_source("ledc.output");
  App.register_component(out_red);
  out_red->set_zero_means_zero(false);
  out_red->set_frequency(5000.0f);
  // output.ledc:
  //   platform: ledc
  //   pin:
  //     number: 4
  //     mode:
  //       output: true
  //       input: false
  //       open_drain: false
  //       pullup: false
  //       pulldown: false
  //     id: esp32_esp32internalgpiopin_id_2
  //     inverted: false
  //     ignore_pin_validation_error: false
  //     ignore_strapping_warning: false
  //     drive_strength: 20.0
  //   id: out_green
  //   frequency: 5000.0
  //   zero_means_zero: false
  esp32_esp32internalgpiopin_id_2 = new esp32::ESP32InternalGPIOPin();
  esp32_esp32internalgpiopin_id_2->set_pin(::GPIO_NUM_4);
  esp32_esp32internalgpiopin_id_2->set_inverted(false);
  esp32_esp32internalgpiopin_id_2->set_drive_strength(::GPIO_DRIVE_CAP_2);
  esp32_esp32internalgpiopin_id_2->set_flags(gpio::Flags::FLAG_OUTPUT);
  out_green = new ledc::LEDCOutput(esp32_esp32internalgpiopin_id_2);
  out_green->set_component_source("ledc.output");
  App.register_component(out_green);
  out_green->set_zero_means_zero(false);
  out_green->set_frequency(5000.0f);
  // output.ledc:
  //   platform: ledc
  //   pin:
  //     number: 3
  //     mode:
  //       output: true
  //       input: false
  //       open_drain: false
  //       pullup: false
  //       pulldown: false
  //     id: esp32_esp32internalgpiopin_id_3
  //     inverted: false
  //     ignore_pin_validation_error: false
  //     ignore_strapping_warning: false
  //     drive_strength: 20.0
  //   id: out_blue
  //   frequency: 5000.0
  //   zero_means_zero: false
  esp32_esp32internalgpiopin_id_3 = new esp32::ESP32InternalGPIOPin();
  esp32_esp32internalgpiopin_id_3->set_pin(::GPIO_NUM_3);
  esp32_esp32internalgpiopin_id_3->set_inverted(false);
  esp32_esp32internalgpiopin_id_3->set_drive_strength(::GPIO_DRIVE_CAP_2);
  esp32_esp32internalgpiopin_id_3->set_flags(gpio::Flags::FLAG_OUTPUT);
  out_blue = new ledc::LEDCOutput(esp32_esp32internalgpiopin_id_3);
  out_blue->set_component_source("ledc.output");
  App.register_component(out_blue);
  out_blue->set_zero_means_zero(false);
  out_blue->set_frequency(5000.0f);
  // light.rgb:
  //   platform: rgb
  //   id: ikea_lamp
  //   name: IKEA Head Lamp
  //   red: out_red
  //   green: out_green
  //   blue: out_blue
  //   default_transition_length: 700ms
  //   restore_mode: ALWAYS_OFF
  //   disabled_by_default: false
  //   gamma_correct: 2.8
  //   flash_transition_length: 0s
  //   output_id: rgb_rgblightoutput_id
  rgb_rgblightoutput_id = new rgb::RGBLightOutput();
  ikea_lamp = new light::LightState(rgb_rgblightoutput_id);
  App.register_light(ikea_lamp);
  ikea_lamp->set_component_source("light");
  App.register_component(ikea_lamp);
  ikea_lamp->set_name("IKEA Head Lamp");
  ikea_lamp->set_object_id("ikea_head_lamp");
  ikea_lamp->set_disabled_by_default(false);
  ikea_lamp->set_restore_mode(light::LIGHT_ALWAYS_OFF);
  ikea_lamp->set_default_transition_length(700);
  ikea_lamp->set_flash_transition_length(0);
  ikea_lamp->set_gamma_correct(2.8f);
  ikea_lamp->add_effects({});
  rgb_rgblightoutput_id->set_red(out_red);
  rgb_rgblightoutput_id->set_green(out_green);
  rgb_rgblightoutput_id->set_blue(out_blue);
  // number.template:
  //   platform: template
  //   id: sunrise_minutes
  //   name: Sunrise Duration Minutes
  //   optimistic: true
  //   restore_value: true
  //   min_value: 10.0
  //   max_value: 60.0
  //   step: 5.0
  //   initial_value: 30.0
  //   disabled_by_default: false
  //   mode: AUTO
  //   update_interval: 60s
  sunrise_minutes = new template_::TemplateNumber();
  sunrise_minutes->set_update_interval(60000);
  sunrise_minutes->set_component_source("template.number");
  App.register_component(sunrise_minutes);
  App.register_number(sunrise_minutes);
  sunrise_minutes->set_name("Sunrise Duration Minutes");
  sunrise_minutes->set_object_id("sunrise_duration_minutes");
  sunrise_minutes->set_disabled_by_default(false);
  sunrise_minutes->traits.set_min_value(10.0f);
  sunrise_minutes->traits.set_max_value(60.0f);
  sunrise_minutes->traits.set_step(5.0f);
  sunrise_minutes->traits.set_mode(number::NUMBER_MODE_AUTO);
  sunrise_minutes->set_optimistic(true);
  sunrise_minutes->set_initial_value(30.0f);
  sunrise_minutes->set_restore_value(true);
  // text_sensor.template:
  //   platform: template
  //   id: last_event
  //   name: Lamp Last Button Event
  //   icon: mdi:gesture-tap
  //   disabled_by_default: false
  //   update_interval: 60s
  last_event = new template_::TemplateTextSensor();
  App.register_text_sensor(last_event);
  last_event->set_name("Lamp Last Button Event");
  last_event->set_object_id("lamp_last_button_event");
  last_event->set_disabled_by_default(false);
  last_event->set_icon("mdi:gesture-tap");
  last_event->set_update_interval(60000);
  last_event->set_component_source("template.text_sensor");
  App.register_component(last_event);
  // sensor.template:
  //   platform: template
  //   id: last_event_time
  //   name: Lamp Last Button Time
  //   device_class: timestamp
  //   disabled_by_default: false
  //   force_update: false
  //   accuracy_decimals: 1
  //   update_interval: 60s
  last_event_time = new template_::TemplateSensor();
  App.register_sensor(last_event_time);
  last_event_time->set_name("Lamp Last Button Time");
  last_event_time->set_object_id("lamp_last_button_time");
  last_event_time->set_disabled_by_default(false);
  last_event_time->set_device_class("timestamp");
  last_event_time->set_accuracy_decimals(1);
  last_event_time->set_force_update(false);
  last_event_time->set_update_interval(60000);
  last_event_time->set_component_source("template.sensor");
  App.register_component(last_event_time);
  // binary_sensor.gpio:
  //   platform: gpio
  //   pin:
  //     number: 5
  //     mode:
  //       input: true
  //       pullup: true
  //       output: false
  //       open_drain: false
  //       pulldown: false
  //     inverted: true
  //     id: esp32_esp32internalgpiopin_id_4
  //     ignore_pin_validation_error: false
  //     ignore_strapping_warning: false
  //     drive_strength: 20.0
  //   id: lamp_button
  //   name: Lamp Button (raw)
  //   filters:
  //     - delayed_on: 15ms
  //       type_id: binary_sensor_delayedonfilter_id
  //     - delayed_off: 15ms
  //       type_id: binary_sensor_delayedofffilter_id
  //   on_press:
  //     - then:
  //         - text_sensor.template.publish:
  //             id: last_event
  //             state: Pressed
  //           type_id: text_sensor_textsensorpublishaction_id
  //         - sensor.template.publish:
  //             id: last_event_time
  //             state: !lambda |-
  //               return id(esptime).now().timestamp;
  //           type_id: sensor_sensorpublishaction_id
  //         - if:
  //             condition:
  //               light.is_on:
  //                 id: ikea_lamp
  //               type_id: light_lightisoncondition_id
  //             then:
  //               - light.turn_off:
  //                   id: ikea_lamp
  //                   state: false
  //                 type_id: light_lightcontrolaction_id
  //             else:
  //               - script.execute:
  //                   id: turn_on_fixed_light
  //                 type_id: script_scriptexecuteaction_id
  //           type_id: ifaction_id
  //       automation_id: automation_id
  //       trigger_id: binary_sensor_presstrigger_id
  //   disabled_by_default: false
  //   use_interrupt: true
  //   interrupt_type: ANY
  lamp_button = new gpio::GPIOBinarySensor();
  App.register_binary_sensor(lamp_button);
  lamp_button->set_name("Lamp Button (raw)");
  lamp_button->set_object_id("lamp_button__raw_");
  lamp_button->set_disabled_by_default(false);
  lamp_button->set_trigger_on_initial_state(false);
  binary_sensor_delayedonfilter_id = new binary_sensor::DelayedOnFilter();
  binary_sensor_delayedonfilter_id->set_component_source("binary_sensor");
  App.register_component(binary_sensor_delayedonfilter_id);
  binary_sensor_delayedonfilter_id->set_delay(15);
  binary_sensor_delayedofffilter_id = new binary_sensor::DelayedOffFilter();
  binary_sensor_delayedofffilter_id->set_component_source("binary_sensor");
  App.register_component(binary_sensor_delayedofffilter_id);
  binary_sensor_delayedofffilter_id->set_delay(15);
  lamp_button->add_filters({binary_sensor_delayedonfilter_id, binary_sensor_delayedofffilter_id});
  binary_sensor_presstrigger_id = new binary_sensor::PressTrigger(lamp_button);
  automation_id = new Automation<>(binary_sensor_presstrigger_id);
  text_sensor_textsensorpublishaction_id = new text_sensor::TextSensorPublishAction<>(last_event);
  text_sensor_textsensorpublishaction_id->set_state("Pressed");
  sensor_sensorpublishaction_id = new sensor::SensorPublishAction<>(last_event_time);
  sensor_sensorpublishaction_id->set_state([=]() -> float {
      #line 116 "ikea_head_lamp.yaml"
      return esptime->now().timestamp;
  });
  light_lightisoncondition_id = new light::LightIsOnCondition<>(ikea_lamp);
  ifaction_id = new IfAction<>(light_lightisoncondition_id);
  light_lightcontrolaction_id = new light::LightControlAction<>(ikea_lamp);
  light_lightcontrolaction_id->set_state(false);
  ifaction_id->add_then({light_lightcontrolaction_id});
  // script:
  //   - id: turn_on_fixed_light
  //     mode: single
  //     then:
  //       - light.turn_on:
  //           id: ikea_lamp
  //           brightness: 0.7
  //           red: 1.0
  //           green: 0.8
  //           blue: 0.6
  //           state: true
  //         type_id: light_lightcontrolaction_id_2
  //     trigger_id: trigger_id
  //     automation_id: automation_id_2
  //     parameters: {}
  //   - id: run_sunrise
  //     mode: restart
  //     then:
  //       - lambda: !lambda |-
  //           id(sunrise_running) = true;
  //           id(sunrise_level) = 0.2f;       START AT 20%
  //           ESP_LOGI("sunrise", "Sunrise starting");
  //         type_id: lambdaaction_id
  //       - light.turn_on:
  //           id: ikea_lamp
  //           brightness: 0.2
  //           red: 1.0
  //           green: 0.4
  //           blue: 0.1
  //           state: true
  //         type_id: light_lightcontrolaction_id_3
  //       - while:
  //           condition:
  //             lambda: !lambda |-
  //               return id(sunrise_running);
  //             type_id: lambdacondition_id
  //           then:
  //             - lambda: !lambda |-
  //                 id(sunrise_level) += 0.01f;
  //                 if (id(sunrise_level) >= 1.0f)
  //                   id(sunrise_running) = false;
  //               type_id: lambdaaction_id_2
  //             - light.turn_on:
  //                 id: ikea_lamp
  //                 brightness: !lambda |-
  //                   return id(sunrise_level);
  //                 state: true
  //               type_id: light_lightcontrolaction_id_4
  //             - delay: !lambda |-
  //                 return (id(sunrise_minutes).state * 60000) / 80;
  //               type_id: delayaction_id
  //         type_id: whileaction_id
  //     trigger_id: trigger_id_2
  //     automation_id: automation_id_3
  //     parameters: {}
  //   - id: stop_sunrise
  //     mode: single
  //     then:
  //       - lambda: !lambda |-
  //           id(sunrise_running) = false;
  //           ESP_LOGI("sunrise", "Sunrise stopped");
  //         type_id: lambdaaction_id_3
  //     trigger_id: trigger_id_3
  //     automation_id: automation_id_4
  //     parameters: {}
  turn_on_fixed_light = new script::SingleScript<>();
  turn_on_fixed_light->set_name("turn_on_fixed_light");
  run_sunrise = new script::RestartScript<>();
  run_sunrise->set_name("run_sunrise");
  stop_sunrise = new script::SingleScript<>();
  stop_sunrise->set_name("stop_sunrise");
  automation_id_2 = new Automation<>(turn_on_fixed_light);
  light_lightcontrolaction_id_2 = new light::LightControlAction<>(ikea_lamp);
  light_lightcontrolaction_id_2->set_state(true);
  light_lightcontrolaction_id_2->set_brightness(0.7f);
  light_lightcontrolaction_id_2->set_red(1.0f);
  light_lightcontrolaction_id_2->set_green(0.8f);
  light_lightcontrolaction_id_2->set_blue(0.6f);
  automation_id_2->add_actions({light_lightcontrolaction_id_2});
  automation_id_3 = new Automation<>(run_sunrise);
  // button.template:
  //   platform: template
  //   name: Start Sunrise
  //   on_press:
  //     - then:
  //         - script.execute:
  //             id: run_sunrise
  //           type_id: script_scriptexecuteaction_id_2
  //       automation_id: automation_id_5
  //       trigger_id: button_buttonpresstrigger_id
  //   disabled_by_default: false
  //   id: template__templatebutton_id
  template__templatebutton_id = new template_::TemplateButton();
  App.register_button(template__templatebutton_id);
  template__templatebutton_id->set_name("Start Sunrise");
  template__templatebutton_id->set_object_id("start_sunrise");
  template__templatebutton_id->set_disabled_by_default(false);
  button_buttonpresstrigger_id = new button::ButtonPressTrigger(template__templatebutton_id);
  automation_id_5 = new Automation<>(button_buttonpresstrigger_id);
  script_scriptexecuteaction_id_2 = new script::ScriptExecuteAction<script::Script<>>(run_sunrise);
  script_scriptexecuteaction_id_2->set_args();
  automation_id_5->add_actions({script_scriptexecuteaction_id_2});
  // button.template:
  //   platform: template
  //   name: Stop Sunrise
  //   on_press:
  //     - then:
  //         - script.execute:
  //             id: stop_sunrise
  //           type_id: script_scriptexecuteaction_id_3
  //       automation_id: automation_id_6
  //       trigger_id: button_buttonpresstrigger_id_2
  //   disabled_by_default: false
  //   id: template__templatebutton_id_2
  template__templatebutton_id_2 = new template_::TemplateButton();
  App.register_button(template__templatebutton_id_2);
  template__templatebutton_id_2->set_name("Stop Sunrise");
  template__templatebutton_id_2->set_object_id("stop_sunrise");
  template__templatebutton_id_2->set_disabled_by_default(false);
  button_buttonpresstrigger_id_2 = new button::ButtonPressTrigger(template__templatebutton_id_2);
  automation_id_6 = new Automation<>(button_buttonpresstrigger_id_2);
  script_scriptexecuteaction_id_3 = new script::ScriptExecuteAction<script::Script<>>(stop_sunrise);
  script_scriptexecuteaction_id_3->set_args();
  automation_id_6->add_actions({script_scriptexecuteaction_id_3});
  // socket:
  //   implementation: bsd_sockets
  // md5:
  script_scriptexecuteaction_id = new script::ScriptExecuteAction<script::Script<>>(turn_on_fixed_light);
  script_scriptexecuteaction_id->set_args();
  ifaction_id->add_else({script_scriptexecuteaction_id});
  automation_id->add_actions({text_sensor_textsensorpublishaction_id, sensor_sensorpublishaction_id, ifaction_id});
  lamp_button->set_component_source("gpio.binary_sensor");
  App.register_component(lamp_button);
  esp32_esp32internalgpiopin_id_4 = new esp32::ESP32InternalGPIOPin();
  esp32_esp32internalgpiopin_id_4->set_pin(::GPIO_NUM_5);
  esp32_esp32internalgpiopin_id_4->set_inverted(true);
  esp32_esp32internalgpiopin_id_4->set_drive_strength(::GPIO_DRIVE_CAP_2);
  esp32_esp32internalgpiopin_id_4->set_flags((gpio::Flags::FLAG_INPUT | gpio::Flags::FLAG_PULLUP));
  lamp_button->set_pin(esp32_esp32internalgpiopin_id_4);
  lamp_button->set_use_interrupt(true);
  lamp_button->set_interrupt_type(gpio::INTERRUPT_ANY_EDGE);
  // globals:
  //   id: sunrise_running
  //   type: bool
  //   restore_value: false
  //   initial_value: 'false'
  sunrise_running = new globals::GlobalsComponent<bool>(false);
  sunrise_running->set_component_source("globals");
  App.register_component(sunrise_running);
  // globals:
  //   id: sunrise_level
  //   type: float
  //   restore_value: false
  //   initial_value: '0.2'
  sunrise_level = new globals::GlobalsComponent<float>(0.2);
  sunrise_level->set_component_source("globals");
  App.register_component(sunrise_level);
  lambdaaction_id = new LambdaAction<>([=]() -> void {
      #line 148 "ikea_head_lamp.yaml"
      sunrise_running->value() = true;
      sunrise_level->value() = 0.2f;       
      ESP_LOGI("sunrise", "Sunrise starting");
  });
  light_lightcontrolaction_id_3 = new light::LightControlAction<>(ikea_lamp);
  light_lightcontrolaction_id_3->set_state(true);
  light_lightcontrolaction_id_3->set_brightness(0.2f);
  light_lightcontrolaction_id_3->set_red(1.0f);
  light_lightcontrolaction_id_3->set_green(0.4f);
  light_lightcontrolaction_id_3->set_blue(0.1f);
  lambdacondition_id = new LambdaCondition<>([=]() -> bool {
      #line 161 "ikea_head_lamp.yaml"
      return sunrise_running->value();
  });
  whileaction_id = new WhileAction<>(lambdacondition_id);
  lambdaaction_id_2 = new LambdaAction<>([=]() -> void {
      #line 164 "ikea_head_lamp.yaml"
      sunrise_level->value() += 0.01f;
      if (sunrise_level->value() >= 1.0f)
        sunrise_running->value() = false;
  });
  light_lightcontrolaction_id_4 = new light::LightControlAction<>(ikea_lamp);
  light_lightcontrolaction_id_4->set_state(true);
  light_lightcontrolaction_id_4->set_brightness([=]() -> float {
      #line 170 "ikea_head_lamp.yaml"
      return sunrise_level->value();
  });
  delayaction_id = new DelayAction<>();
  delayaction_id->set_component_source("script");
  App.register_component(delayaction_id);
  delayaction_id->set_delay([=]() -> uint32_t {
      #line 174 "ikea_head_lamp.yaml"
      return (sunrise_minutes->state * 60000) / 80;
  });
  whileaction_id->add_then({lambdaaction_id_2, light_lightcontrolaction_id_4, delayaction_id});
  automation_id_3->add_actions({lambdaaction_id, light_lightcontrolaction_id_3, whileaction_id});
  automation_id_4 = new Automation<>(stop_sunrise);
  lambdaaction_id_3 = new LambdaAction<>([=]() -> void {
      #line 181 "ikea_head_lamp.yaml"
      sunrise_running->value() = false;
      ESP_LOGI("sunrise", "Sunrise stopped");
  });
  automation_id_4->add_actions({lambdaaction_id_3});
  // =========== AUTO GENERATED CODE END ============
  App.setup();
}

void loop() {
  App.loop();
}
