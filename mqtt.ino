void setupMQTT() {
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(mqttCallback);
}
// Fungsi untuk memulai mode tertentu
void startRFIDMode(RFIDMode mode, String name = "") {
  currentRFIDMode = mode;
  pendingName = name;
  pendingUID = "";
  modeTimeout = millis() + MODE_TIMEOUT_MS;
  
  // Kirim notifikasi MQTT
  sendModeStatus();
  
  Serial.print("🔧 RFID Mode: ");
  switch(mode) {
    case RFID_MODE_ADD:
      Serial.println("ADD - Tunggu tap kartu baru");
      break;
    case RFID_MODE_EDIT:
      Serial.println("EDIT - Tunggu tap kartu yang akan diedit");
      break;
    case RFID_MODE_DELETE:
      Serial.println("DELETE - Tunggu tap kartu yang akan dihapus");
      break;
    default:
      Serial.println("NORMAL");
      break;
  }
}
void publishRFIDScan(String uid, String name = "", bool registered = false) {
  if (!client.connected()) return;
  
  DynamicJsonDocument doc(512);
  doc["id"] = deviceId;
  doc["action"] = "rfid_scan";
  doc["uid"] = uid;
  doc["registered"] = registered;
  doc["timestamp"] = millis();
  
  if (registered && name != "") {
    doc["name"] = name;
  } else {
    doc["name"] = "Unknown";
  }
  
  String jsonStr;
  serializeJson(doc, jsonStr);
  
  client.publish("starprint/rfid/scan", jsonStr.c_str());
  Serial.println("📤 RFID scan event terkirim");
}
// Fungsi untuk mengirim response RFID
void sendRFIDResponse(String status, String message, String uid = "", String name = "") {
  DynamicJsonDocument doc(512);
  doc["id"] = deviceId;
  doc["action"] = "rfid_response";
  doc["status"] = status;
  doc["message"] = message;
  
  if (uid != "") {
    doc["uid"] = uid;
  }
  
  if (name != "") {
    doc["name"] = name;
  }
  
  doc["timestamp"] = sysTime.datetime;
  doc["timestamp_unix"] = sysTime.unix; 
  
  String jsonStr;
  serializeJson(doc, jsonStr);
  
  client.publish("starprint/rfid/response", jsonStr.c_str());
  Serial.println("📤 RFID response terkirim: " + jsonStr);
}
// Fungsi Handle Koneksi MQTT
void handleMQTTConnection() {
  if (!client.connected()) {
    reconnectMQTT();
  }
}

void reconnectMQTT() {
  int attempts = 0;
  while (!client.connected() && attempts < 5) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(deviceId.c_str())) {
      Serial.println("connected");
      
      // Subscribe ke topic yang diperlukan
      client.subscribe("starprint/device/#");
      client.subscribe("starprint/rfid/#");
      
      Serial.println("✅ Subscribed to RFID topics");
      
      attempts = 0;
      return;
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
      attempts++;
    }
  }
  
  // Jika gagal connect setelah beberapa percobaan, restart ESP
  if (attempts >= 5) {
    Serial.println("MQTT connection failed, restarting...");
    delay(1000);
    ESP.restart();
  }
}

// PERBAIKAN: Hapus "void" yang duplikat
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("📨 Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);

  // Parse JSON utama
  DynamicJsonDocument doc(256);
  DeserializationError error = deserializeJson(doc, message);
  
  if (error) {
    Serial.println("❌ Failed to parse MQTT message");
    return;
  }

  String id = doc["id"].as<String>();  // PERBAIKAN: Tambahkan .as<String>()
  String action = doc["action"].as<String>();
  String state = doc["state"].as<String>();
  String deviceKeyFromMqtt = doc["device_key"].as<String>();

  // Validasi ID perangkat
  if (id != deviceId && deviceKeyFromMqtt != deviceKey) {
    Serial.println("⚠️ ID tidak sesuai, abaikan pesan");
    return;
  }

  // Proses kontrol relay
  if (action == "relay_control") {
    if (state == "ON") {
      AccessState = ACCESS_GRANTED;
      // setRelayState(true);
      beepRequest = 1;
      Serial.println("🔌 Relay ON");
    } 
    else if (state == "OFF") {
      AccessState = ACCESS_DENIED;
      beepRequest = 1;
      // setRelayState(false);
      Serial.println("🔌 Relay OFF");
    }
  }
  // Reset ESP
  else if (action == "esp_control" && state == "reset") {
    Serial.println("🔄 Reset request received.");
    ESP.restart();
  }
  // Handle RFID management
  else if (action.startsWith("rfid_")) {
    handleRFIDManagement(message);
  }
}

// Fungsi Send Status Acknowledgment
void sendStatusAck(String state) {
  DynamicJsonDocument doc(256);
  doc["id"] = deviceId;
  doc["status"] = "acknowledged";
  doc["state"] = state;
  doc["timestamp"] = millis();

  String jsonStr;
  serializeJson(doc, jsonStr);
  
  client.publish("smartpower/device/status", jsonStr.c_str());
  Serial.println("Status terkirim: " + jsonStr);
}

void handleRFIDManagement(String message) {
  DynamicJsonDocument doc(512);
  DeserializationError error = deserializeJson(doc, message);
  
  if (error) {
    Serial.println("❌ Failed to parse RFID management JSON");
    return;
  }

  String receivedId = doc["id"].as<String>();
  if (receivedId != deviceId) {
    Serial.println("❌ ID tidak sesuai");
    return;
  }

  String action = doc["action"].as<String>();
  
  // Mode tap operations
  if (action == "rfid_mode_add") {
    beepRequest = 3;
    handleAddRFIDMode(doc);
  } 
  else if (action == "rfid_mode_edit") {
    beepRequest = 3;
    handleEditRFIDMode(doc);
  }
  else if (action == "rfid_mode_delete") {
    beepRequest = 3;
    handleDeleteRFIDMode(doc);
  }
  else if (action == "rfid_mode_cancel") {
    beepRequest = 1;
    exitRFIDMode();
    sendRFIDResponse("success", "Mode dibatalkan");
  }
  else if (action == "rfid_add") {
    beepRequest = 3;
    handleAddRFID(doc);
  } 
  else if (action == "rfid_remove") {
    beepRequest = 3;
    handleRemoveRFID(doc);
  }
  else if (action == "rfid_edit") {
    beepRequest = 3;
    handleEditRFID(doc);
  }
  else if (action == "rfid_clear") {
    beepRequest = 3;
    handleClearRFID(doc);
  }
  else if (action == "rfid_list") {
    beepRequest = 4;
    handleListRFID(doc);
  }
  else if (action == "rfid_check") {
    beepRequest = 4;
    handleCheckRFID(doc);
  }
}