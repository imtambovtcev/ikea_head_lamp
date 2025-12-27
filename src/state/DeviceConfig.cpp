#include "DeviceConfig.h"

const char* DeviceConfig::NVS_NAMESPACE = "ikea_head_lamp";

DeviceConfig::DeviceConfig() 
  : defaultBrightness(70),
    defaultColorR(255),
    defaultColorG(147),
    defaultColorB(41),
    sunriseMinutes(30),
    sunriseFinalBrightness(100),
    minPwmPercent(20),
    maxPwmPercent(100),
    favoriteAnimation("fire"),
    favAnimParam1(70),      // Default: fire intensity
    favAnimParam2(5),       // Default: fire speed
    favAnimParam3(0),
    favAnimColorR(0),
    favAnimColorG(0),
    favAnimColorB(0),
    version(1) {
}

void DeviceConfig::load() {
  Preferences prefs;
  prefs.begin(NVS_NAMESPACE, true); // read-only

  defaultBrightness = prefs.getUChar("def_bri", defaultBrightness);
  defaultColorR     = prefs.getUChar("def_r",   defaultColorR);
  defaultColorG     = prefs.getUChar("def_g",   defaultColorG);
  defaultColorB     = prefs.getUChar("def_b",   defaultColorB);

  sunriseMinutes         = prefs.getUShort("sun_min", sunriseMinutes);
  sunriseFinalBrightness = prefs.getUChar("sun_bri",  sunriseFinalBrightness);

  minPwmPercent = prefs.getUChar("min_pwm", minPwmPercent);
  maxPwmPercent = prefs.getUChar("max_pwm", maxPwmPercent);

  // Load favorite animation settings
  favoriteAnimation = prefs.getString("fav_anim", favoriteAnimation);
  favAnimParam1 = prefs.getUChar("fav_p1", favAnimParam1);
  favAnimParam2 = prefs.getUChar("fav_p2", favAnimParam2);
  favAnimParam3 = prefs.getUChar("fav_p3", favAnimParam3);
  favAnimColorR = prefs.getUChar("fav_r", favAnimColorR);
  favAnimColorG = prefs.getUChar("fav_g", favAnimColorG);
  favAnimColorB = prefs.getUChar("fav_b", favAnimColorB);

  version = prefs.getUInt("cfg_ver", version);

  prefs.end();

  clampValues();

  Serial.println("[CFG] Loaded from NVS:");
  Serial.printf("      defaultBrightness=%u\n", defaultBrightness);
  Serial.printf("      defaultColor=(%u,%u,%u)\n", defaultColorR, defaultColorG, defaultColorB);
  Serial.printf("      sunriseMinutes=%u, sunriseFinalBrightness=%u\n",
                sunriseMinutes, sunriseFinalBrightness);
  Serial.printf("      minPwm=%u%%, maxPwm=%u%%\n",
                minPwmPercent, maxPwmPercent);
  Serial.printf("      favoriteAnimation=%s, params=(%u,%u,%u), color=(%u,%u,%u)\n",
                favoriteAnimation.c_str(), favAnimParam1, favAnimParam2, favAnimParam3,
                favAnimColorR, favAnimColorG, favAnimColorB);
  Serial.printf("      version=%lu\n", (unsigned long)version);
}

void DeviceConfig::save() {
  Preferences prefs;
  prefs.begin(NVS_NAMESPACE, false); // write mode

  version++;

  prefs.putUChar("def_bri", defaultBrightness);
  prefs.putUChar("def_r",   defaultColorR);
  prefs.putUChar("def_g",   defaultColorG);
  prefs.putUChar("def_b",   defaultColorB);

  prefs.putUShort("sun_min", sunriseMinutes);
  prefs.putUChar("sun_bri",  sunriseFinalBrightness);

  prefs.putUChar("min_pwm", minPwmPercent);
  prefs.putUChar("max_pwm", maxPwmPercent);

  // Save favorite animation settings
  prefs.putString("fav_anim", favoriteAnimation);
  prefs.putUChar("fav_p1", favAnimParam1);
  prefs.putUChar("fav_p2", favAnimParam2);
  prefs.putUChar("fav_p3", favAnimParam3);
  prefs.putUChar("fav_r", favAnimColorR);
  prefs.putUChar("fav_g", favAnimColorG);
  prefs.putUChar("fav_b", favAnimColorB);

  prefs.putUInt("cfg_ver", version);

  prefs.end();

  Serial.printf("[CFG] Saved to NVS. New version=%lu\n", (unsigned long)version);
}

void DeviceConfig::reset() {
  Serial.println("[CFG] Resetting to built-in defaults");
  *this = DeviceConfig(); // Reset to constructor defaults
  version++;
  save();
}

void DeviceConfig::clampValues() {
  if (defaultBrightness > 100) defaultBrightness = 100;
  if (defaultBrightness < 20)  defaultBrightness = 70;

  if (sunriseMinutes < 5)   sunriseMinutes = 30;
  if (sunriseMinutes > 180) sunriseMinutes = 60;

  if (sunriseFinalBrightness < 20)  sunriseFinalBrightness = 100;
  if (sunriseFinalBrightness > 100) sunriseFinalBrightness = 100;

  if (minPwmPercent > 100) minPwmPercent = 20;
  if (maxPwmPercent > 100) maxPwmPercent = 100;
  if (maxPwmPercent <= minPwmPercent) maxPwmPercent = 100;
}
