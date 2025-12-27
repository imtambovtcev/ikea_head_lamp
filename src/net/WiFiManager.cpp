#include "WiFiManager.h"
#include "wifi_config.h"
#include "../hw/StatusLED.h"

WiFiManager::WiFiManager() 
  : statusLED(nullptr), wasConnected(false), lastReconnectAttempt(0) {
}

void WiFiManager::begin() {
  Serial.println("[WIFI] Starting WiFi manager");
  WiFi.mode(WIFI_STA);
  connect();
}

void WiFiManager::loop() {
  bool isConnected = (WiFi.status() == WL_CONNECTED);

  if (!isConnected && wasConnected) {
    Serial.println("[WIFI] Connection lost");
    wasConnected = false;
  }

  if (!isConnected) {
    unsigned long now = millis();
    if (now - lastReconnectAttempt > RECONNECT_INTERVAL_MS) {
      lastReconnectAttempt = now;
      connect();
    }
  } else if (!wasConnected) {
    wasConnected = true;
  }
}

bool WiFiManager::connected() {
  return WiFi.status() == WL_CONNECTED;
}

void WiFiManager::setStatusLED(StatusLED* led) {
  statusLED = led;
}

void WiFiManager::connect() {
  Serial.printf("[WIFI] Connecting to %s\n", WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  uint8_t dots = 0;
  while (WiFi.status() != WL_CONNECTED) {
    if (statusLED) statusLED->wifiConnecting();
    delay(500);
    Serial.print(".");
    if (++dots % 20 == 0) Serial.println();
    
    // Timeout after 30 seconds
    if (dots > 60) {
      Serial.println("\n[WIFI] Connection timeout");
      if (statusLED) statusLED->wifiFailed();
      return;
    }
  }

  Serial.println();
  Serial.printf("[WIFI] Connected. IP=%s\n", WiFi.localIP().toString().c_str());
  if (statusLED) statusLED->wifiConnected();
  wasConnected = true;
}
