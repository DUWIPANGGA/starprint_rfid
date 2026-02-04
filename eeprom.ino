void initEEPROM() {
  EEPROM.begin(EEPROM_SIZE);
}
void writeStringEEPROM(int addr, const String &data) {
  int len = data.length();

  if (addr + len + 1 >= EEPROM_SIZE) {
    Serial.println("❌ EEPROM overflow");
    return;
  }

  EEPROM.write(addr, len);
  for (int i = 0; i < len; i++) {
    EEPROM.write(addr + 1 + i, data[i]);
  }

  EEPROM.commit();
}

/**
 * Read string based on length byte
 */
String readStringEEPROM(int addr) {
  int len = EEPROM.read(addr);

  if (len <= 0 || len > 100) return "";

  char data[len + 1];

  for (int i = 0; i < len; i++) {
    data[i] = EEPROM.read(addr + 1 + i);
  }
  data[len] = '\0';

  return String(data);
}

void clearEEPROM() {
  for (int i = 0; i < EEPROM_SIZE; i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
}