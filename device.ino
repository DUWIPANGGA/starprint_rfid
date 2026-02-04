

// ===== DEVICE ID =====
void generateDeviceId() {
  String mac = WiFi.macAddress();
  mac.replace(":", "");
  mac.toUpperCase();

  deviceId = mac;

  Serial.print("🆔 Device ID: ");
  Serial.println(deviceId);
}

String getDeviceId() {
  return deviceId;
}

// ===== ESP CONTROL =====
void restartESP(uint16_t delayMs) {
  Serial.println("🔄 Restarting ESP...");
  delay(delayMs);
  ESP.restart();
}

void factoryReset() {
  Serial.println("⚠ Factory reset started");

  EEPROM.begin(EEPROM_SIZE);
  for (int i = 0; i < EEPROM_SIZE; i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
  EEPROM.end();

  Serial.println("✅ EEPROM cleared");
  delay(1000);
  ESP.restart();
}

// ===== INFO =====
void printDeviceInfo() {
  Serial.println("====== DEVICE INFO ======");
  Serial.print("Device ID : ");
  Serial.println(deviceId);

  Serial.print("MAC       : ");
  Serial.println(WiFi.macAddress());

  Serial.print("Free Heap : ");
  Serial.print(getFreeHeap());
  Serial.println(" bytes");

  Serial.print("Uptime    : ");
  Serial.print(getUptime());
  Serial.println(" sec");

  Serial.println("=========================");
}

uint32_t getFreeHeap() {
  return ESP.getFreeHeap();
}

uint32_t getUptime() {
  return millis() / 1000;
}
