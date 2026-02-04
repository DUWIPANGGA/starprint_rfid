// Handler untuk mode add (dari MQTT)
void handleAddRFIDMode(DynamicJsonDocument& doc) {
  String name = doc["name"].as<String>();
  
  if (name == "") {
    sendRFIDResponse("error", "Nama diperlukan untuk mode add");
    return;
  }
  
  startRFIDMode(RFID_MODE_ADD, name);
  sendRFIDResponse("success", "Mode ADD aktif. Tap kartu dalam 30 detik", "", name);
}

// Handler untuk mode edit (dari MQTT)
void handleEditRFIDMode(DynamicJsonDocument& doc) {
  String name = doc["name"].as<String>();
  
  if (name == "") {
    sendRFIDResponse("error", "Nama baru diperlukan untuk mode edit");
    return;
  }
  
  startRFIDMode(RFID_MODE_EDIT, name);
  sendRFIDResponse("success", "Mode EDIT aktif. Tap kartu dalam 30 detik", "", name);
}

// Handler untuk mode delete (dari MQTT)
void handleDeleteRFIDMode(DynamicJsonDocument& doc) {
  startRFIDMode(RFID_MODE_DELETE);
  sendRFIDResponse("success", "Mode DELETE aktif. Tap kartu dalam 30 detik");
}
// Handler untuk menambah RFID
void handleAddRFID(DynamicJsonDocument& doc) {
  String uid = doc["uid"];
  String name = doc["name"];
  
  if (uid == "" || name == "") {
    sendRFIDResponse("error", "UID atau nama tidak valid");
    return;
  }
  
  if (addRFIDKey(uid, name)) {
    Serial.printf("✅ RFID ditambahkan: %s -> %s\n", uid.c_str(), name.c_str());
    sendRFIDResponse("success", "RFID berhasil ditambahkan", uid, name);
  } else {
    Serial.println("❌ Gagal menambahkan RFID");
    sendRFIDResponse("error", "Gagal menambahkan RFID");
  }
}

// Handler untuk menghapus RFID
void handleRemoveRFID(DynamicJsonDocument& doc) {
  String uid = doc["uid"];
  
  if (uid == "") {
    sendRFIDResponse("error", "UID tidak valid");
    return;
  }
  
  if (removeRFIDKey(uid)) {
    Serial.printf("✅ RFID dihapus: %s\n", uid.c_str());
    sendRFIDResponse("success", "RFID berhasil dihapus", uid);
  } else {
    Serial.printf("❌ Gagal menghapus RFID: %s\n", uid.c_str());
    sendRFIDResponse("error", "RFID tidak ditemukan");
  }
}

// Handler untuk mengedit RFID
void handleEditRFID(DynamicJsonDocument& doc) {
  String uid = doc["uid"];
  String newName = doc["name"];
  
  if (uid == "" || newName == "") {
    sendRFIDResponse("error", "UID atau nama baru tidak valid");
    return;
  }
  
  if (renameRFIDKey(uid, newName)) {
    Serial.printf("✅ RFID diubah: %s -> %s\n", uid.c_str(), newName.c_str());
    sendRFIDResponse("success", "RFID berhasil diubah", uid, newName);
  } else {
    Serial.printf("❌ Gagal mengubah RFID: %s\n", uid.c_str());
    sendRFIDResponse("error", "RFID tidak ditemukan");
  }
}

// Handler untuk menghapus semua RFID
void handleClearRFID(DynamicJsonDocument& doc) {
  if (clearAllRFIDKeys()) {
    Serial.println("✅ Semua RFID dihapus");
    sendRFIDResponse("success", "Semua RFID berhasil dihapus");
  } else {
    Serial.println("❌ Gagal menghapus semua RFID");
    sendRFIDResponse("error", "Gagal menghapus semua RFID");
  }
}

// Handler untuk mengecek RFID
void handleCheckRFID(DynamicJsonDocument& doc) {
  String uid = doc["uid"];
  
  if (uid == "") {
    sendRFIDResponse("error", "UID tidak valid");
    return;
  }
  
  bool registered = isRFIDRegistered(uid);
  String name = getRFIDName(uid);
  
  DynamicJsonDocument response(512);
  response["id"] = deviceId;
  response["action"] = "rfid_check_response";
  response["status"] = registered ? "registered" : "not_registered";
  response["uid"] = uid;
  
  if (registered) {
    response["name"] = name;
  }
  
  response["timestamp"] = millis();
  
  String jsonStr;
  serializeJson(response, jsonStr);
  
  client.publish("starprint/rfid/response", jsonStr.c_str());
  Serial.println("📤 RFID check response terkirim");
}

// Handler untuk mendapatkan daftar RFID
void handleListRFID(DynamicJsonDocument& doc) {
  int count = countRFIDKeys();

  DynamicJsonDocument response(2048);
  response["id"] = deviceId;
  response["action"] = "rfid_list_response";
  response["total"] = count;
  response["timestamp"] = millis();

  JsonArray rfidList = response.createNestedArray("rfid_cards");

  // isi langsung
  fillRFIDList(rfidList);

  if (client.connected()) {
    String jsonStr;
    serializeJson(response, jsonStr);
    client.publish("starprint/rfid/list", jsonStr.c_str());
  }

  Serial.printf("📤 RFID list terkirim: %d cards\n", count);
}
void fillRFIDList(JsonArray arr) {
  StaticJsonDocument<JSON_SIZE> doc;
  if (!loadConfig(doc)) return;

  JsonObject list = doc["list_key"].as<JsonObject>();
  if (list.isNull()) return;

  for (JsonPair kv : list) {
    JsonObject card = arr.createNestedObject();
    card["uid"]  = kv.key();
    card["name"] = kv.value().as<String>();
  }
}


// Fungsi untuk keluar dari mode
void exitRFIDMode() {
  currentRFIDMode = RFID_MODE_NORMAL;
  pendingUID = "";
  pendingName = "";
  modeTimeout = 0;
  
  // Kirim notifikasi
  sendModeStatus();
  
  Serial.println("✅ Kembali ke mode NORMAL");
}

// Kirim status mode ke MQTT
void sendModeStatus() {
  DynamicJsonDocument doc(256);
  doc["id"] = deviceId;
  doc["action"] = "rfid_mode_status";
  
  switch(currentRFIDMode) {
    case RFID_MODE_NORMAL:
      doc["mode"] = "normal";
      doc["message"] = "Mode normal - scan untuk akses";
      break;
    case RFID_MODE_ADD:
      doc["mode"] = "add";
      doc["message"] = "Mode tambah - tunggu tap kartu baru";
      doc["pending_name"] = pendingName;
      break;
    case RFID_MODE_EDIT:
      doc["mode"] = "edit";
      doc["message"] = "Mode edit - tunggu tap kartu yang akan diedit";
      doc["pending_name"] = pendingName;
      break;
    case RFID_MODE_DELETE:
      doc["mode"] = "delete";
      doc["message"] = "Mode delete - tunggu tap kartu yang akan dihapus";
      break;
  }
  
  doc["timeout"] = modeTimeout;
  doc["timestamp"] = millis();
  
  String jsonStr;
  serializeJson(doc, jsonStr);
  client.publish("starprint/rfid/mode", jsonStr.c_str());
}
