bool initLittleFS() {
  if (!LittleFS.begin(true)) {
    Serial.println("❌ LittleFS mount failed");
    return false;
  }
  Serial.println("✅ LittleFS mounted");
  return true;
}

bool loadConfig(StaticJsonDocument<JSON_SIZE>& doc) {
  if (!LittleFS.exists(CONFIG_FILE)) {
    saveDefaultConfig();
  }

  File file = LittleFS.open(CONFIG_FILE, "r");
  if (!file) return false;

  DeserializationError err = deserializeJson(doc, file);
  file.close();

  if (err) {
    Serial.println("❌ JSON parse error");
    return false;
  }

  return true;
}

String getConfigString(const char* key) {
  StaticJsonDocument<JSON_SIZE> doc;
  if (!loadConfig(doc)) return "";

  return doc[key] | "";
}

int getConfigInt(const char* key) {
  StaticJsonDocument<JSON_SIZE> doc;
  if (!loadConfig(doc)) return 0;

  return doc[key] | 0;
}

void setConfigValue(const char* key, const char* value) {
  updateJson(key, value);
}

void setConfigValue(const char* key, String value) {
  updateJson(key, value.c_str());
}

void setConfigValue(const char* key, int value) {
  updateJson(key, value);
}

void setConfigValue(const char* key, bool value) {
  updateJson(key, value);
}

bool deleteConfigKey(const char* key) {
  StaticJsonDocument<JSON_SIZE> doc;
  if (!loadConfig(doc)) return false;

  doc.remove(key);

  File file = LittleFS.open(CONFIG_FILE, "w");
  serializeJsonPretty(doc, file);
  file.close();

  return true;
}

void printConfig() {
  StaticJsonDocument<JSON_SIZE> doc;
  if (!loadConfig(doc)) return;

  serializeJsonPretty(doc, Serial);
  Serial.println();
}

void saveDefaultConfig() {
  StaticJsonDocument<JSON_SIZE> doc;

  doc["wifi_ssid"] = "";
  doc["wifi_pass"] = "";
  doc["ap_name"] = "Starprint Smart RFID";
  doc["ap_pass"] = "88888888";
  doc["mqtt_host"] = "broker.emqx.io";
  doc["mqtt_port"] = 1883;
  doc["interval"]  = 5000;
  doc["mode"]      = "auto";
  
  // Buat objek kosong untuk list_key
  JsonObject listKeys = doc.createNestedObject("list_key");
  
  // (Opsional) Tambahkan beberapa contoh RFID key
  listKeys["F1894101"] = "Administrator";
  listKeys["18823599"] = "User Card #1";

  File file = LittleFS.open(CONFIG_FILE, "w");
  serializeJsonPretty(doc, file);
  file.close();

  Serial.println("📝 Default config created");
}

void updateJson(const char* key, const char* value) {
  StaticJsonDocument<JSON_SIZE> doc;
  if (!loadConfig(doc)) return;

  doc[key] = value;

  File file = LittleFS.open(CONFIG_FILE, "w");
  serializeJsonPretty(doc, file);
  file.close();
}

void updateJson(const char* key, int value) {
  StaticJsonDocument<JSON_SIZE> doc;
  if (!loadConfig(doc)) return;

  doc[key] = value;

  File file = LittleFS.open(CONFIG_FILE, "w");
  serializeJsonPretty(doc, file);
  file.close();
}

void updateJson(const char* key, bool value) {
  StaticJsonDocument<JSON_SIZE> doc;
  if (!loadConfig(doc)) return;

  doc[key] = value;

  File file = LittleFS.open(CONFIG_FILE, "w");
  serializeJsonPretty(doc, file);
  file.close();
}

void setupVariableFromLittleFS(){
  ap_name=getConfigString("ap_name");
  ap_pass=getConfigString("ap_pass");
}

// ====================================================================
// RFID LIST MANAGEMENT FUNCTIONS (TAMBAHAN)
// ====================================================================

// Fungsi helper untuk menyimpan konfigurasi (versi singkat)
bool saveConfigDoc(StaticJsonDocument<JSON_SIZE>& doc) {
  File file = LittleFS.open(CONFIG_FILE, "w");
  if (!file) return false;
  
  serializeJsonPretty(doc, file);
  file.close();
  
  return true;
}

// Fungsi untuk mendapatkan semua RFID keys yang tersimpan
JsonObject getAllRFIDKeys() {
  StaticJsonDocument<JSON_SIZE> doc;
  if (!loadConfig(doc)) return JsonObject();
  
  if (!doc.containsKey("list_key") || !doc["list_key"].is<JsonObject>()) {
    // Jika belum ada, buat objek kosong
    doc.createNestedObject("list_key");
    saveConfigDoc(doc);
    return doc["list_key"].as<JsonObject>();
  }
  
  return doc["list_key"].as<JsonObject>();
}

// Fungsi untuk menambah/memperbarui key RFID dengan nama
bool addRFIDKey(String uid, String name) {
  StaticJsonDocument<JSON_SIZE> doc;
  if (!loadConfig(doc)) return false;
  
  // Pastikan list_key ada
  if (!doc.containsKey("list_key")) {
    doc.createNestedObject("list_key");
  }
  
  // Tambahkan/update key
  JsonObject listKeys = doc["list_key"].as<JsonObject>();
  listKeys[uid] = name;
  
  return saveConfigDoc(doc);
}

// Fungsi untuk menghapus key RFID
bool removeRFIDKey(String uid) {
  StaticJsonDocument<JSON_SIZE> doc;
  if (!loadConfig(doc)) return false;
  
  // Cek apakah list_key ada
  if (!doc.containsKey("list_key") || !doc["list_key"].is<JsonObject>()) {
    return false;
  }
  
  JsonObject listKeys = doc["list_key"].as<JsonObject>();
  if (!listKeys.containsKey(uid)) {
    return false;
  }
  
  listKeys.remove(uid);
  return saveConfigDoc(doc);
}

// Fungsi untuk mendapatkan nama berdasarkan UID
String getRFIDName(String uid) {
  StaticJsonDocument<JSON_SIZE> doc;
  if (!loadConfig(doc)) return "";
  
  if (!doc.containsKey("list_key") || !doc["list_key"].is<JsonObject>()) {
    return "";
  }
  
  JsonObject listKeys = doc["list_key"].as<JsonObject>();
  if (listKeys.containsKey(uid)) {
    return listKeys[uid].as<String>();
  }
  
  return "";
}

// Fungsi untuk mengecek apakah UID terdaftar
bool isRFIDRegistered(String uid) {
  StaticJsonDocument<JSON_SIZE> doc;
  if (!loadConfig(doc)) return false;
  
  if (!doc.containsKey("list_key") || !doc["list_key"].is<JsonObject>()) {
    return false;
  }
  
  JsonObject listKeys = doc["list_key"].as<JsonObject>();
  return listKeys.containsKey(uid);
}

// Fungsi untuk menghitung jumlah key RFID yang terdaftar
int countRFIDKeys() {
  StaticJsonDocument<JSON_SIZE> doc;
  if (!loadConfig(doc)) return 0;
  
  if (!doc.containsKey("list_key") || !doc["list_key"].is<JsonObject>()) {
    return 0;
  }
  
  JsonObject listKeys = doc["list_key"].as<JsonObject>();
  return listKeys.size();
}

// Fungsi untuk menghapus semua key RFID
bool clearAllRFIDKeys() {
  StaticJsonDocument<JSON_SIZE> doc;
  if (!loadConfig(doc)) return false;
  
  if (doc.containsKey("list_key")) {
    doc.remove("list_key");
    doc.createNestedObject("list_key"); // Buat objek kosong
    return saveConfigDoc(doc);
  }
  
  return false;
}

// Fungsi untuk mendapatkan array UID yang terdaftar
void getRegisteredUIDs(String uidArray[], int maxSize, int &count) {
  count = 0;
  StaticJsonDocument<JSON_SIZE> doc;
  if (!loadConfig(doc)) return;
  
  if (!doc.containsKey("list_key") || !doc["list_key"].is<JsonObject>()) {
    return;
  }
  
  JsonObject listKeys = doc["list_key"].as<JsonObject>();
  
  for (JsonPair kv : listKeys) {
    if (count < maxSize) {
      uidArray[count] = String(kv.key().c_str());
      count++;
    }
  }
}

// Fungsi untuk mengubah nama RFID key
bool renameRFIDKey(String uid, String newName) {
  // Cek apakah UID ada
  if (!isRFIDRegistered(uid)) {
    return false;
  }
  
  // Update nama
  return addRFIDKey(uid, newName);
}

// Fungsi untuk mencetak semua RFID keys
void printAllRFIDKeys() {
  JsonObject listKeys = getAllRFIDKeys();
  
  Serial.println("📋 Registered RFID Cards:");
  Serial.println("========================================");
  
  if (listKeys.size() == 0) {
    Serial.println("No cards registered");
    return;
  }
  
  for (JsonPair kv : listKeys) {
    Serial.printf("UID: %s -> Name: %s\n", 
                  kv.key().c_str(), 
                  kv.value().as<String>().c_str());
  }
  Serial.println("========================================");
}