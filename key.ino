String shuffleString(String input) {
  int len = input.length();
  for (int i = 0; i < len; i++) {
    int r = random(len);
    char temp = input[i];
    input[i] = input[r];
    input[r] = temp;
  }
  return input;
}
String generateKeyFromDeviceId() {
  randomSeed(esp_random() ^ millis());

  String base = deviceId;
  base = shuffleString(base);

  String randomPart = "";
  const char hex[] = "0123456789ABCDEF";

  for (int i = 0; i < 8; i++) {    
    randomPart += hex[random(16)];
  }

  String key = base + randomPart;

  // potong / sesuaikan panjang
  if (key.length() > KEY_LENGTH) {
    key = key.substring(0, KEY_LENGTH);
  }

  return key;
}
void saveKeyToEEPROM(String key) {
  EEPROM.begin(EEPROM_SIZE);
  for (int i = 0; i < KEY_LENGTH; i++) {
    EEPROM.write(KEY_ADDR + i, key[i]);
  }
  EEPROM.commit();
  EEPROM.end();
}
String loadKeyFromEEPROM() {
  EEPROM.begin(EEPROM_SIZE);
  String key = "";

  for (int i = 0; i < KEY_LENGTH; i++) {
    char c = EEPROM.read(KEY_ADDR + i);
    if (c == 0 || c == 255) {
      EEPROM.end();
      return "";
    }
    key += c;
  }

  EEPROM.end();
  return key;
}
String getKey() {
  if (deviceKey.length() > 0) {
    return deviceKey;
  }

  deviceKey = loadKeyFromEEPROM();

  if (deviceKey == "") {
    Serial.println("🔑 Generating key from Device ID...");
    deviceKey = generateKeyFromDeviceId();
    saveKeyToEEPROM(deviceKey);

    Serial.print("✅ Key generated & saved: ");
    Serial.println(deviceKey);
  } else {
    Serial.print("🔓 Key loaded from EEPROM: ");
    Serial.println(deviceKey);
  }

  return deviceKey;
}
void regenerateKey() {
  Serial.println("♻ Regenerating key...");
  deviceKey = generateKeyFromDeviceId();
  saveKeyToEEPROM(deviceKey);

  Serial.print("🔑 New Key: ");
  Serial.println(deviceKey);
}

