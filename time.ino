// ===== FUNGSI WAKTU =====

// Fungsi untuk inisialisasi waktu
void initTime() {
  Serial.println("Initializing time system...");
  
  // Konfigurasi NTP
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  
  // Coba dapatkan waktu dari NTP
  if (updateTimeFromNTP()) {  
    timeInitialized = true;
    Serial.println("Time initialized successfully from NTP");
  } else {
    // Jika gagal, gunakan waktu default
    currentUnixTime = 1609459200; // 1 Januari 2021 00:00:00 UTC
    updateTimeString();
    timeInitialized = false;
    Serial.println("Time initialized with default value");
  }
  
  // Simpan waktu startup
  startupUnixTime = currentUnixTime;
  startupTimeString = currentTimeString;
  
  Serial.print("Startup time: ");
  Serial.println(startupTimeString);
  Serial.print("Startup Unix: ");
  Serial.println(startupUnixTime);
}

// Fungsi untuk mendapatkan waktu dari NTP
bool updateTimeFromNTP() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo, 5000)) { // Timeout 5 detik
    Serial.println("Failed to get time from NTP");
    return false;
  }
  
  // Konversi ke Unix timestamp
  time_t now;
  time(&now);
  currentUnixTime = now;
  
  // Update string time
  updateTimeString();
  
  return true;
}

// Fungsi untuk update string waktu dari Unix timestamp
void updateTimeString() {
  time_t rawtime = currentUnixTime;
  struct tm *timeinfo;
  
  timeinfo = localtime(&rawtime);
  
  // Format lengkap: YYYY-MM-DD HH:MM:SS
  char buffer[20];
  strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
  currentTimeString = String(buffer);
  
  // Update sysTime struct
  sysTime.unix = currentUnixTime;
  sysTime.datetime = currentTimeString;
  
  // Date only
  strftime(buffer, sizeof(buffer), "%Y-%m-%d", timeinfo);
  sysTime.date = String(buffer);
  
  // Time only
  strftime(buffer, sizeof(buffer), "%H:%M:%S", timeinfo);
  sysTime.time = String(buffer);
  
  // ISO 8601 format
  strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%S%z", timeinfo);
  sysTime.iso8601 = String(buffer);
  
  // Untuk log
  sysTime.timestamp = "[" + currentTimeString + "] ";
}

// Fungsi untuk update waktu secara periodic
void updateTime() {
  if (millis() - lastTimeUpdate >= TIME_UPDATE_INTERVAL) {
    lastTimeUpdate = millis();
    
    if (timeInitialized && WiFi.status() == WL_CONNECTED) {
      // Jika terhubung WiFi, cek NTP setiap 1 jam
      static unsigned long lastNTPCheck = 0;
      if (millis() - lastNTPCheck >= 3600000) { // 1 jam
        lastNTPCheck = millis();
        updateTimeFromNTP();
      } else {
        // Tambahkan 1 detik ke Unix timestamp
        currentUnixTime++;
        updateTimeString();
      }
    } else {
      // Jika tidak terhubung, tambahkan 1 detik saja
      currentUnixTime++;
      updateTimeString();
    }
  }
}

// Fungsi untuk mendapatkan timestamp untuk log MQTT
String getMQTTTimestamp() {
  return sysTime.timestamp;
}

// Fungsi untuk mendapatkan waktu dalam format JSON (untuk MQTT message)
String getTimeJSON() {
  DynamicJsonDocument doc(256);
  doc["unix"] = sysTime.unix;
  doc["datetime"] = sysTime.datetime;
  doc["date"] = sysTime.date;
  doc["time"] = sysTime.time;
  doc["iso8601"] = sysTime.iso8601;
  doc["startup_unix"] = startupUnixTime;
  doc["startup_datetime"] = startupTimeString;
  doc["uptime"] = millis() / 1000; // Uptime dalam detik
  
  String jsonString;
  serializeJson(doc, jsonString);
  return jsonString;
}

// Fungsi untuk membuat log message dengan timestamp
String createLogMessage(const String& message, const String& type = "INFO") {
  String logMsg = sysTime.timestamp;
  logMsg += "[";
  logMsg += type;
  logMsg += "] ";
  logMsg += message;
  return logMsg;
}

// Fungsi untuk publish MQTT dengan timestamp otomatis
void publishWithTimestamp(const String& topic, const String& message, bool retain = false) {
  if (client.connected()) {
    // Buat payload dengan timestamp
    DynamicJsonDocument doc(512);
    doc["timestamp"] = sysTime.datetime;
    doc["timestamp_unix"] = sysTime.unix;
    doc["message"] = message;
    doc["device_id"] = deviceId;
    
    String payload;
    serializeJson(doc, payload);
    
    client.publish(topic.c_str(), payload.c_str(), retain);
    
    // Log ke serial
    Serial.print(getMQTTTimestamp());
    Serial.print("MQTT Published to ");
    Serial.print(topic);
    Serial.print(": ");
    Serial.println(message);
  }
}

// Fungsi untuk publish log event
void publishLogEvent(const String& event, const String& details = "") {
  String topic = "device/" + deviceId + "/log";
  
  DynamicJsonDocument doc(512);
  doc["timestamp"] = sysTime.datetime;
  doc["event"] = event;
  doc["device_id"] = deviceId;
  
  if (details.length() > 0) {
    doc["details"] = details;
  }
  
  // Tambahkan info tambahan berdasarkan event
  if (event == "rfid_scan") {
    doc["last_uid"] = lastUID;
    doc["access_state"] = AccessState == ACCESS_GRANTED ? "granted" : 
                         AccessState == ACCESS_DENIED ? "denied" : "idle";
  } else if (event == "door_status") {
    doc["door_state"] = doorState;
    doc["door_open"] = doorStatus;
  } else if (event == "relay_state") {
    doc["relay_on"] = digitalRead(RELAY_PIN);
  }
  
  String payload;
  serializeJson(doc, payload);
  
  client.publish(topic.c_str(), payload.c_str());
}

// Fungsi untuk sync waktu via MQTT command
void handleTimeSyncCommand(const String& command) {
  if (command == "sync") {
    if (updateTimeFromNTP()) {
      publishWithTimestamp("device/" + deviceId + "/time/sync", "Time synchronized successfully");
    } else {
      publishWithTimestamp("device/" + deviceId + "/time/sync", "Time synchronization failed", false);
    }
  } else if (command == "get") {
    // Kirim waktu saat ini
    publishWithTimestamp("device/" + deviceId + "/time/current", getTimeJSON());
  } else if (command.startsWith("set:")) {
    // Format: set:UNIX_TIMESTAMP
    String timestampStr = command.substring(4);
    unsigned long newTime = timestampStr.toInt();
    
    if (newTime > 1609459200) { // Validasi: setelah 1 Jan 2021
      currentUnixTime = newTime;
      updateTimeString();
      timeInitialized = true;
      publishWithTimestamp("device/" + deviceId + "/time/set", "Time set manually");
    }
  }
}