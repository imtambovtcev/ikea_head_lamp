#include "LampHardware.h"

LampHardware::LampHardware() {
}

void LampHardware::begin() {
  Serial.println("[HW] Initializing PWM channels");

  ledcSetup(PWM_CHANNEL_RED,   PWM_FREQ, PWM_BITS);
  ledcSetup(PWM_CHANNEL_GREEN, PWM_FREQ, PWM_BITS);
  ledcSetup(PWM_CHANNEL_BLUE,  PWM_FREQ, PWM_BITS);

  ledcAttachPin(PIN_RED,   PWM_CHANNEL_RED);
  ledcAttachPin(PIN_GREEN, PWM_CHANNEL_GREEN);
  ledcAttachPin(PIN_BLUE,  PWM_CHANNEL_BLUE);

  // Start with all LEDs off
  ledcWrite(PWM_CHANNEL_RED,   0);
  ledcWrite(PWM_CHANNEL_GREEN, 0);
  ledcWrite(PWM_CHANNEL_BLUE,  0);
}

void LampHardware::apply(bool power, uint8_t brightness,
                          uint8_t r, uint8_t g, uint8_t b,
                          uint8_t minPwmPercent, uint8_t maxPwmPercent) {
  if (!power) {
    ledcWrite(PWM_CHANNEL_RED,   0);
    ledcWrite(PWM_CHANNEL_GREEN, 0);
    ledcWrite(PWM_CHANNEL_BLUE,  0);
    return;
  }

  float physicalPercent = logicalToPhysical(brightness, minPwmPercent, maxPwmPercent);
  
  uint8_t adjR = (uint8_t)round(r * physicalPercent);
  uint8_t adjG = (uint8_t)round(g * physicalPercent);
  uint8_t adjB = (uint8_t)round(b * physicalPercent);

  uint32_t maxDuty = (1UL << PWM_BITS) - 1;
  uint32_t dutyR = (uint32_t)round((adjR / 255.0f) * maxDuty);
  uint32_t dutyG = (uint32_t)round((adjG / 255.0f) * maxDuty);
  uint32_t dutyB = (uint32_t)round((adjB / 255.0f) * maxDuty);

  ledcWrite(PWM_CHANNEL_RED,   dutyR);
  ledcWrite(PWM_CHANNEL_GREEN, dutyG);
  ledcWrite(PWM_CHANNEL_BLUE,  dutyB);

  Serial.printf("[HW] Applied: power=%d, bri=%u, phys=%.2f%%, RGB=(%u,%u,%u), duty=(%lu,%lu,%lu)\n",
                power, brightness, physicalPercent * 100.0f,
                adjR, adjG, adjB, dutyR, dutyG, dutyB);
}

float LampHardware::logicalToPhysical(uint8_t logical, uint8_t minPwm, uint8_t maxPwm) {
  if (logical == 0) {
    return 0.0f;
  }

  float l = logical / 100.0f;
  float minP = minPwm / 100.0f;
  float maxP = maxPwm / 100.0f;

  if (minP < 0.0f) minP = 0.0f;
  if (maxP > 1.0f) maxP = 1.0f;
  if (maxP < minP) maxP = minP;

  return minP + l * (maxP - minP);
}
