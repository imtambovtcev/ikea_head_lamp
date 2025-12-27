#include "SystemMonitor.h"

SystemMonitor::SystemMonitor() 
  : bootTime(0), loopCount(0), minFreeHeap(0xFFFFFFFF) {
}

void SystemMonitor::begin() {
  bootTime = millis();
  resetReason = esp_reset_reason();
  minFreeHeap = ESP.getFreeHeap();

  Serial.println("\n[SYS] ============================================");
  Serial.println("[SYS] System Monitor Initialized");
  Serial.printf("[SYS] Reset reason: %s\n", getResetReason().c_str());
  Serial.printf("[SYS] Free heap: %u bytes\n", ESP.getFreeHeap());
  Serial.printf("[SYS] Chip model: %s\n", ESP.getChipModel());
  Serial.printf("[SYS] CPU freq: %u MHz\n", ESP.getCpuFreqMHz());
  Serial.println("[SYS] ============================================\n");
}

void SystemMonitor::update() {
  uint32_t currentHeap = ESP.getFreeHeap();
  if (currentHeap < minFreeHeap) {
    minFreeHeap = currentHeap;
  }
}

unsigned long SystemMonitor::getUptimeSeconds() const {
  return (millis() - bootTime) / 1000;
}

uint32_t SystemMonitor::getFreeHeap() const {
  return ESP.getFreeHeap();
}

uint32_t SystemMonitor::getMinFreeHeap() const {
  return minFreeHeap;
}

String SystemMonitor::getResetReason() const {
  switch(resetReason) {
    case ESP_RST_POWERON:   return "POWERON";
    case ESP_RST_SW:        return "SOFTWARE";
    case ESP_RST_PANIC:     return "PANIC_EXCEPTION";
    case ESP_RST_INT_WDT:   return "INTERRUPT_WATCHDOG";
    case ESP_RST_TASK_WDT:  return "TASK_WATCHDOG";
    case ESP_RST_WDT:       return "OTHER_WATCHDOG";
    case ESP_RST_BROWNOUT:  return "BROWNOUT";
    case ESP_RST_SDIO:      return "SDIO";
    default:                
      char buf[32];
      snprintf(buf, sizeof(buf), "UNKNOWN_%d", (int)resetReason);
      return String(buf);
  }
}

unsigned long SystemMonitor::getLoopCount() const {
  return loopCount;
}

void SystemMonitor::incrementLoop() {
  loopCount++;
}
