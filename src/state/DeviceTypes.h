#ifndef DEVICE_TYPES_H
#define DEVICE_TYPES_H

#include <Arduino.h>

/**
 * Operating modes for the lamp.
 */
enum class LampMode : uint8_t {
  STATIC = 0,     // Solid color/brightness
  ANIMATION = 1   // Running an animation
};

#endif // DEVICE_TYPES_H
