
// ====================================================================
// RFID INITIALIZATION
// ====================================================================

void initRFID() {
  SPI.begin();
  mfrc522.PCD_Init();

  // Optional: Increase antenna gain (range 0x00 to 0x07)
  mfrc522.PCD_SetAntennaGain(MFRC522::RxGain_max);

  Serial.println(F("RFID Reader Initialized"));
  Serial.println(F("Tap RFID/NFC tag on the reader..."));
  Serial.println();
}

// ====================================================================
// CARD DETECTION FUNCTIONS
// ====================================================================

bool isCardPresent() {
  // Check for new cards
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return false;
  }
  return true;
}

bool readCardSerial() {
  // Try to read the card serial
  if (!mfrc522.PICC_ReadCardSerial()) {
    return false;
  }
  return true;
}

bool isCardValid() {
  return (isCardPresent() && readCardSerial());
}

// ====================================================================
// UID FORMATTING FUNCTIONS
// ====================================================================

String getUIDWithSpaces() {
  String uid = "";

  for (byte i = 0; i < mfrc522.uid.size; i++) {
    // Add leading zero if needed
    if (mfrc522.uid.uidByte[i] < 0x10) {
      uid += "0";
    }

    // Add hex value
    uid += String(mfrc522.uid.uidByte[i], HEX);

    // Add space between bytes (except last)
    if (i < mfrc522.uid.size - 1) {
      uid += " ";
    }
  }

  uid.toUpperCase();  // Convert to uppercase
  return uid;
}

String getUIDCompact() {
  String uid = "";

  for (byte i = 0; i < mfrc522.uid.size; i++) {
    // Ensure 2-digit hex format
    if (mfrc522.uid.uidByte[i] < 0x10) {
      uid += "0";
    }

    // Add hex value
    uid += String(mfrc522.uid.uidByte[i], HEX);
  }

  uid.toUpperCase();  // Convert to uppercase
  return uid;
}

String getUIDDecimal() {
  String uid = "";

  for (byte i = 0; i < mfrc522.uid.size; i++) {
    uid += String(mfrc522.uid.uidByte[i]);

    // Add dash between bytes (except last)
    if (i < mfrc522.uid.size - 1) {
      uid += "-";
    }
  }

  return uid;
}

String getUIDByteArray() {
  String uid = "{";

  for (byte i = 0; i < mfrc522.uid.size; i++) {
    uid += "0x";

    // Add leading zero if needed
    if (mfrc522.uid.uidByte[i] < 0x10) {
      uid += "0";
    }

    uid += String(mfrc522.uid.uidByte[i], HEX);

    // Add comma between bytes (except last)
    if (i < mfrc522.uid.size - 1) {
      uid += ", ";
    }
  }

  uid += "}";
  return uid;
}

// ====================================================================
// CARD INFORMATION FUNCTIONS
// ====================================================================

int getCardSize() {
  return mfrc522.uid.size;
}

String getCardType() {
  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  return String(mfrc522.PICC_GetTypeName(piccType));
}

void displayCardInfo() {
  Serial.println(F("\n========================================"));
  Serial.println(F("CARD INFORMATION"));
  Serial.println(F("========================================"));

  // Display UID in different formats
  Serial.println(F("UID Formats:"));
  Serial.print(F("  HEX with spaces : "));
  Serial.println(getUIDWithSpaces());
  Serial.print(F("  HEX compact     : "));
  Serial.println(getUIDCompact());
  Serial.print(F("  Decimal         : "));
  Serial.println(getUIDDecimal());
  Serial.print(F("  Byte array      : "));
  Serial.println(getUIDByteArray());

  // Display card details
  Serial.println(F("\nCard Details:"));
  Serial.print(F("  Size           : "));
  Serial.print(getCardSize());
  Serial.println(F(" bytes"));
  Serial.print(F("  Type           : "));
  Serial.println(getCardType());
  Serial.println(F("----------------------------------------"));
}

// ====================================================================
// DISPLAY FUNCTIONS
// ====================================================================

void displayAuthorizationResult(bool authorized, String cardName = "") {
  if (authorized) {
    Serial.print(F("✅ ACCESS GRANTED"));
    if (cardName != "") {
      Serial.print(F(" - "));
      Serial.print(cardName);
    }
  } else {
    Serial.print(F("❌ ACCESS DENIED"));
  }
  Serial.println();
}

void displayError(String message) {
  Serial.print(F("⚠️ ERROR: "));
  Serial.println(message);
}

// ====================================================================
// AUTHORIZATION FUNCTIONS
// ====================================================================

bool checkAuthorizationByBytes(byte authorizedUID[], int uidSize) {
  // Check if sizes match
  if (mfrc522.uid.size != uidSize) {
    return false;
  }

  // Compare each byte
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    if (mfrc522.uid.uidByte[i] != authorizedUID[i]) {
      return false;
    }
  }

  return true;
}

bool checkAuthorizationByString(String authorizedUIDs[], int numCards, String &cardName) {
  String currentUID = getUIDCompact();

  for (int i = 0; i < numCards; i++) {
    if (currentUID == authorizedUIDs[i]) {
      cardName = "Card #" + String(i + 1);
      return true;
    }
  }

  cardName = "";
  return false;
}

void haltCard() {
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}

String getTimestamp() {
  unsigned long totalSeconds = millis() / 1000;
  unsigned long hours = totalSeconds / 3600;
  unsigned long minutes = (totalSeconds % 3600) / 60;
  unsigned long seconds = totalSeconds % 60;

  char timestamp[12];
  sprintf(timestamp, "%02lu:%02lu:%02lu", hours, minutes, seconds);

  return String(timestamp);
}

void logAccessAttempt(bool authorized, String cardName) {
  Serial.print(F("📝 LOG: ["));
  Serial.print(getTimestamp());
  Serial.print(F("] "));
  Serial.print(authorized ? F("GRANTED") : F("DENIED"));
  Serial.print(F(" - UID: "));
  Serial.print(getUIDCompact());

  if (authorized && cardName != "") {
    Serial.print(F(" ("));
    Serial.print(cardName);
    Serial.print(F(")"));
  }

  Serial.println();
}
// ====================================================================
// CARD PROCESSING FUNCTIONS (REVISI)
// ====================================================================

void processCard() {
  // Dapatkan UID
  String currentUID = getUIDCompact();
  currentUID.toUpperCase();

  // Tampilkan info kartu (opsional, bisa dikomentari jika tidak perlu)
  // displayCardInfo();

  // Handle berdasarkan mode
  switch (currentRFIDMode) {
    case RFID_MODE_ADD:
      Serial.println("🔧 Mode ADD: Processing card...");
      handleAddByTap(currentUID);
      break;

    case RFID_MODE_EDIT:
      Serial.println("🔧 Mode EDIT: Processing card...");
      handleEditByTap(currentUID);
      break;

    case RFID_MODE_DELETE:
      Serial.println("🔧 Mode DELETE: Processing card...");
      handleDeleteByTap(currentUID);
      break;

    case RFID_MODE_NORMAL:
    default:
      // Mode normal - cek akses
      String cardName = getRFIDName(currentUID);
      bool isAuthorized = isRFIDRegistered(currentUID);

      if (isAuthorized) {
        Serial.println(F("✅ Kartu terdaftar"));
        Serial.print(F("Nama: "));
        Serial.println(cardName);
        AccessState = ACCESS_GRANTED;
        beepRequest = 1;
        // sendAccessLog(currentUID, cardName, true);
      } else {
        Serial.println(F("❌ Kartu tidak dikenal"));
        Serial.print(F("UID: "));
        Serial.println(currentUID);
        beepRequest = 2;
        AccessState = ACCESS_DENIED;
        // sendAccessLog(currentUID, cardName, true);
      }

      // Log the access attempt
      logAccessAttempt(isAuthorized, cardName);

      // Kirim ke MQTT untuk mode normal
      if (client.connected()) {
        publishRFIDScan(currentUID, cardName, isAuthorized);
      }
      break;
  }

  // Halt the card
  haltCard();
}

// ====================================================================
// MAIN LOOP FUNCTION
// ====================================================================
void loopCard() {
  // Reset reader berkala
  static unsigned long lastReset = 0;
  // if (millis() - lastReset > 10000) {
  //   mfrc522.PCD_Init();
  //   lastReset = millis();
  // }

  // Cek timeout mode
  if (currentRFIDMode != RFID_MODE_NORMAL && millis() > modeTimeout) {
    Serial.println("⏰ Mode timeout, kembali ke normal");

    // Kirim timeout notification ke MQTT
    if (client.connected()) {
      DynamicJsonDocument doc(256);
      doc["id"] = deviceId;
      doc["action"] = "rfid_mode_timeout";
      doc["mode"] = "timeout";
      doc["message"] = "Mode timeout, tidak ada kartu yang di-scan";
      doc["timestamp"] = millis();

      String jsonStr;
      serializeJson(doc, jsonStr);
      client.publish("starprint/rfid/operation", jsonStr.c_str());
    }

    exitRFIDMode();
    beepRequest = 2;
  }

  // Nonaktifkan tap jika mode normal dan CAN_TAP false
  if (currentRFIDMode == RFID_MODE_NORMAL && !CAN_TAP) {
    return;
  }

  // Cek kartu baru
  // if (!mfrc522.PICC_IsNewCardPresent()) {
  //   return;
  // }
  if (!mfrc522.PICC_IsNewCardPresent()) {
    if (millis() - lastReset > 15000) {
      mfrc522.PCD_Init();
      lastReset = millis();
    }
    return;
  }
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  String currentUID = getUIDCompact();
  currentUID.toUpperCase();

  // Anti double tap
  if (currentUID == lastUID && (millis() - lastReadTime < CARD_COOLDOWN)) {
    haltCard();  // tetap halt biar reader bersih
    return;
  }

  // Update cache
  lastUID = currentUID;
  lastReadTime = millis();

  // Proses kartu
  processCard();
}
// Fungsi utama untuk handle RFID scan
void handleRFIDScan(String uid) {
  // Cek timeout mode
  if (currentRFIDMode != RFID_MODE_NORMAL && millis() > modeTimeout) {
    Serial.println("⏰ Mode timeout, kembali ke normal");
    exitRFIDMode();
  }

  // Handle berdasarkan mode
  switch (currentRFIDMode) {
    case RFID_MODE_ADD:
      handleAddByTap(uid);
      break;

    case RFID_MODE_EDIT:
      handleEditByTap(uid);
      break;

    case RFID_MODE_DELETE:
      handleDeleteByTap(uid);
      break;

    case RFID_MODE_NORMAL:
    default:
      handleNormalScan(uid);
      break;
  }

  // Kirim event scan ke MQTT
  publishRFIDScan(uid);
}

// Handle scan normal (untuk akses)
void handleNormalScan(String uid) {
  bool registered = isRFIDRegistered(uid);
  String name = getRFIDName(uid);

  if (registered) {
    Serial.printf("✅ Akses diterima: %s (%s)\n", uid.c_str(), name.c_str());
    // Lakukan aksi akses (buka pintu, nyalakan relay, dll)
    beepRequest = 1;
  } else {
    Serial.printf("❌ Akses ditolak: %s\n", uid.c_str());
    beepRequest = 2;
  }
}

// Handle add by tap
void handleAddByTap(String uid) {
  if (addRFIDKey(uid, pendingName)) {
    Serial.printf("✅ Kartu ditambahkan: %s -> %s\n", uid.c_str(), pendingName.c_str());

    // Kirim response ke MQTT
    DynamicJsonDocument doc(512);
    doc["id"] = deviceId;
    doc["action"] = "rfid_add_complete";
    doc["status"] = "success";
    doc["uid"] = uid;
    doc["name"] = pendingName;
    doc["message"] = "Kartu berhasil ditambahkan";
    doc["timestamp"] = millis();

    String jsonStr;
    serializeJson(doc, jsonStr);
    client.publish("starprint/rfid/operation", jsonStr.c_str());

    // Beri feedback buzzer
    beepRequest = 1;

    // Kembali ke mode normal
    exitRFIDMode();

  } else {
    Serial.println("❌ Gagal menambahkan kartu");
    beepRequest = 2;
  }
}

// Handle edit by tap
void handleEditByTap(String uid) {
  if (renameRFIDKey(uid, pendingName)) {
    Serial.printf("✅ Kartu diedit: %s -> %s\n", uid.c_str(), pendingName.c_str());

    // Kirim response ke MQTT
    DynamicJsonDocument doc(512);
    doc["id"] = deviceId;
    doc["action"] = "rfid_edit_complete";
    doc["status"] = "success";
    doc["uid"] = uid;
    doc["name"] = pendingName;
    doc["message"] = "Kartu berhasil diedit";
    doc["timestamp"] = millis();

    String jsonStr;
    serializeJson(doc, jsonStr);
    client.publish("starprint/rfid/operation", jsonStr.c_str());

    // Beri feedback buzzer
    beepRequest = 1;

    // Kembali ke mode normal
    exitRFIDMode();

  } else {
    Serial.printf("❌ Kartu tidak ditemukan: %s\n", uid.c_str());

    DynamicJsonDocument doc(512);
    doc["id"] = deviceId;
    doc["action"] = "rfid_edit_complete";
    doc["status"] = "error";
    doc["uid"] = uid;
    doc["message"] = "Kartu tidak ditemukan";
    doc["timestamp"] = millis();

    String jsonStr;
    serializeJson(doc, jsonStr);
    client.publish("starprint/rfid/operation", jsonStr.c_str());

    beepRequest = 2;
  }
}

// Handle delete by tap
void handleDeleteByTap(String uid) {
  String currentName = getRFIDName(uid);

  if (removeRFIDKey(uid)) {
    Serial.printf("✅ Kartu dihapus: %s (%s)\n", uid.c_str(), currentName.c_str());

    // Kirim response ke MQTT
    DynamicJsonDocument doc(512);
    doc["id"] = deviceId;
    doc["action"] = "rfid_delete_complete";
    doc["status"] = "success";
    doc["uid"] = uid;
    doc["name"] = currentName;
    doc["message"] = "Kartu berhasil dihapus";
    doc["timestamp"] = millis();

    String jsonStr;
    serializeJson(doc, jsonStr);
    client.publish("starprint/rfid/operation", jsonStr.c_str());

    // Beri feedback buzzer
    beepRequest = 1;

    // Kembali ke mode normal
    exitRFIDMode();

  } else {
    Serial.printf("❌ Kartu tidak ditemukan: %s\n", uid.c_str());

    DynamicJsonDocument doc(512);
    doc["id"] = deviceId;
    doc["action"] = "rfid_delete_complete";
    doc["status"] = "error";
    doc["uid"] = uid;
    doc["message"] = "Kartu tidak ditemukan";
    doc["timestamp"] = millis();

    String jsonStr;
    serializeJson(doc, jsonStr);
    client.publish("starprint/rfid/operation", jsonStr.c_str());

    beepRequest = 2;
  }
}