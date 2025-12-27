#ifndef SYSTEM_MONITOR_H
#define SYSTEM_MONITOR_H

#include <Arduino.h>
#include "esp_system.h"

/**
 * System health monitoring for remote diagnostics.
 * 
 * Responsibilities:
 * - Track uptime
 * - Monitor free heap
 * - Detect WiFi/MQTT issues
 * - Provide reset reason
 */
class SystemMonitor {
public:
  SystemMonitor();

  /**
   * Initialize and log boot reason.
   */
  void begin();

  /**
   * Update monitoring (call in loop).
   */
  void update();

  /**
   * Get uptime in seconds.
   */
  unsigned long getUptimeSeconds() const;

  /**
   * Get free heap in bytes.
   */
  uint32_t getFreeHeap() const;

  /**
   * Get minimum free heap since boot.
   */
  uint32_t getMinFreeHeap() const;

  /**
   * Get reset reason as string.
   */
  String getResetReason() const;

  /**
   * Get loop iteration count.
   */
  unsigned long getLoopCount() const;

  /**
   * Increment loop counter.
   */
  void incrementLoop();

private:
  unsigned long bootTime;
  unsigned long loopCount;
  uint32_t minFreeHeap;
  esp_reset_reason_t resetReason;
};

#endif // SYSTEM_MONITOR_H
