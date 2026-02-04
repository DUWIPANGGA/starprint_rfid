// ===== API ENDPOINTS =====
void setupWebServer() {
  // Serve HTML page
  server.on("/", HTTP_GET, []() {
    String html = FPSTR(html_page);
    html.replace("%DEVICE_ID%", deviceId);
    server.send(200, "text/html", html);
  });

  // API: Scan WiFi networks
  server.on("/api/scan", HTTP_GET, []() {
    Serial.println("🔍 Mulai scan WiFi...");
    WiFi.scanDelete();
    delay(100);

    int n = WiFi.scanNetworks();
    Serial.printf("📡 Ditemukan %d jaringan\n", n);

    DynamicJsonDocument doc(1024);
    JsonArray networks = doc.createNestedArray("networks");

    for (int i = 0; i < n; i++) {
      JsonObject network = networks.createNestedObject();
      network["ssid"] = WiFi.SSID(i);
      network["rssi"] = WiFi.RSSI(i);
      network["encryption"] = (WiFi.encryptionType(i) != WIFI_AUTH_OPEN);
    }

    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
  });

  // API: Connect to WiFi
  // API: Connect to WiFi
server.on("/api/connect", HTTP_POST, []() {
  if (server.hasArg("plain")) {
    DynamicJsonDocument doc(256);
    deserializeJson(doc, server.arg("plain"));

    String ssid = doc["ssid"].as<String>();
    String password = doc["password"].as<String>();

    Serial.print("Connecting to: ");
    Serial.println(ssid);

    WiFi.begin(ssid.c_str(), password.c_str());

    unsigned long startTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startTime < 15000) {
      delay(500);
      Serial.print(".");
    }

    DynamicJsonDocument responseDoc(128);
    if (WiFi.status() == WL_CONNECTED) {
      responseDoc["success"] = true;
      responseDoc["ip"] = WiFi.localIP().toString();

      // Save to LittleFS
      setConfigValue("wifi_ssid", ssid);
      setConfigValue("wifi_pass", password);
      
      // Save to EEPROM juga untuk kompatibilitas
      saveWiFiCredentials(ssid, password);

      // Setup MQTT dan OTA
      setupMQTT();
      setupOTA();
      checkForOTAUpdate();
      
      // Kirim response sebelum restart
      String response;
      serializeJson(responseDoc, response);
      server.send(200, "application/json", response);
      
      // Tunggu sebentar untuk response dikirim
      delay(500);
      
      // Restart ESP untuk keluar dari AP mode
      Serial.println("WiFi connected, restarting to switch to STA mode...");
      ESP.restart();
      
    } else {
      responseDoc["success"] = false;
      responseDoc["message"] = "Connection timeout";

      String response;
      serializeJson(responseDoc, response);
      server.send(200, "application/json", response);
    }
  } else {
    server.send(400, "application/json", "{\"error\":\"No data\"}");
  }
});

  // API: Reset WiFi configuration
  server.on("/api/reset", HTTP_POST, []() {
    // Clear saved credentials
    setConfigValue("wifi_ssid", "");
    setConfigValue("wifi_pass", "");

    server.send(200, "application/json", "{\"success\":true}");

    // Restart device
    delay(1000);
    ESP.restart();
  });

  // API: Get connection status
  server.on("/api/status", HTTP_GET, []() {
    DynamicJsonDocument doc(256);
    doc["connected"] = (WiFi.status() == WL_CONNECTED);
    doc["deviceId"] = deviceId;
    doc["apMode"] = inAPMode;

    if (WiFi.status() == WL_CONNECTED) {
      doc["ip"] = WiFi.localIP().toString();
      doc["ssid"] = WiFi.SSID();
    }

    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
  });

  // API: Get device info
  server.on("/api/info", HTTP_GET, []() {
    DynamicJsonDocument doc(512);
    doc["deviceId"] = deviceId;
    doc["firmware"] = "1.0.0";
    doc["chipId"] = deviceId;
    doc["flashSize"] = ESP.getFlashChipSize();
    doc["freeHeap"] = ESP.getFreeHeap();
    doc["uptime"] = millis() / 1000;

    // Add WiFi info
    if (WiFi.status() == WL_CONNECTED) {
      doc["wifiStatus"] = "Connected";
      doc["ipAddress"] = WiFi.localIP().toString();
      doc["rssi"] = WiFi.RSSI();
    } else {
      doc["wifiStatus"] = "Disconnected";
      doc["ipAddress"] = "192.168.4.1";
    }

    // Add AP info
    doc["apStatus"] = inAPMode ? "Active" : "Inactive";
    doc["apName"] = getConfigString("ap_name");
    doc["memory"] = String(ESP.getFreeHeap() / 1024) + "KB free";

    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
  });

  // API: Get device details with key
  server.on("/api/detail", HTTP_GET, []() {
    DynamicJsonDocument doc(1024);
    doc["deviceId"] = deviceId;
    doc["firmware"] = "1.0.0";
    doc["chipId"] = deviceId;
    doc["flashSize"] = ESP.getFlashChipSize();
    doc["freeHeap"] = ESP.getFreeHeap();
    doc["uptime"] = millis() / 1000;

    // Add WiFi info
    if (WiFi.status() == WL_CONNECTED) {
      doc["wifiStatus"] = "Connected";
      doc["ipAddress"] = WiFi.localIP().toString();
      doc["rssi"] = WiFi.RSSI();
    } else {
      doc["wifiStatus"] = "Disconnected";
      doc["ipAddress"] = "192.168.4.1";
    }

    // Add AP info
    doc["apStatus"] = inAPMode ? "Active" : "Inactive";
    doc["apName"] = getConfigString("ap_name");
    doc["memory"] = String(ESP.getFreeHeap() / 1024) + "KB free";
    doc["deviceKey"] = getKey();

    // Load additional config
    doc["ap_name"] = getConfigString("ap_name");
    doc["wifi_ssid"] = getConfigString("wifi_ssid");
    doc["wifi_pass"] = getConfigString("wifi_pass");
    doc["mqtt_host"] = getConfigString("mqtt_host");
    doc["mqtt_port"] = getConfigInt("mqtt_port");
    doc["uptime"] = millis() / 1000;
    
  Serial.println(F("\n===== /api/detail JSON ====="));
  serializeJsonPretty(doc, Serial);
  Serial.println(F("\n============================"));

    String response;
    serializeJsonPretty(doc, response);
    server.send(200, "application/json", response);
  });

  // API: Save AP settings
  server.on("/api/ap/settings", HTTP_POST, []() {
    if (server.hasArg("plain")) {
      DynamicJsonDocument doc(256);
      deserializeJson(doc, server.arg("plain"));

      String apSsid = doc["ssid"].as<String>();
      String apPassword = doc["password"].as<String>();
      int apChannel = doc["channel"] | 6;
      bool apEnabled = doc["enabled"] | true;

      // Validate
      if (apSsid.length() == 0) {
        server.send(400, "application/json", "{\"error\":\"AP name required\"}");
        return;
      }

      if (apPassword.length() > 0 && apPassword.length() < 8) {
        server.send(400, "application/json", "{\"error\":\"Password min 8 chars\"}");
        return;
      }

      // Save to LittleFS
      setConfigValue("ap_name", apSsid);
      setConfigValue("ap_pass", apPassword);
      setConfigValue("ap_channel", apChannel);
      setConfigValue("ap_enabled", apEnabled);

      DynamicJsonDocument responseDoc(128);
      responseDoc["success"] = true;
      responseDoc["message"] = "AP settings saved";

      String response;
      serializeJson(responseDoc, response);
      server.send(200, "application/json", response);
    } else {
      server.send(400, "application/json", "{\"error\":\"No data\"}");
    }
  });

  // API: Regenerate device key
  server.on("/api/key/regenerate", HTTP_POST, []() {
    regenerateKey();

    DynamicJsonDocument doc(128);
    doc["success"] = true;
    doc["key"] = deviceKey;
    doc["message"] = "Key regenerated successfully";

    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
  });

  // API: Get device key
  server.on("/api/key/get", HTTP_GET, []() {
    DynamicJsonDocument doc(128);
    doc["success"] = true;
    doc["key"] = getKey();

    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
  });

  // API: Get key info (without revealing key)
  server.on("/api/key/info", HTTP_GET, []() {
    DynamicJsonDocument doc(128);
    doc["success"] = true;
    doc["hasKey"] = (deviceKey.length() > 0);
    doc["keyLength"] = deviceKey.length();
    doc["keyAge"] = "unknown";  // You might want to store key generation timestamp

    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
  });

  // API: Restart device
  server.on("/api/restart", HTTP_POST, []() {
    DynamicJsonDocument doc(128);
    doc["success"] = true;
    doc["message"] = "Device will restart in 1 second";

    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);

    delay(1000);
    ESP.restart();
  });

  // Control Relay via API
  server.on("/api/relay", HTTP_POST, []() {
    if (server.hasArg("plain")) {
      DynamicJsonDocument doc(128);
      deserializeJson(doc, server.arg("plain"));

      if (doc.containsKey("state")) {
        bool newState = doc["state"];
        // setRelayState(newState);

        DynamicJsonDocument responseDoc(64);
        responseDoc["success"] = true;
        responseDoc["state"] = newState;

        String response;
        serializeJson(responseDoc, response);
        server.send(200, "application/json", response);
      } else {
        server.send(400, "application/json", "{\"error\":\"Missing state parameter\"}");
      }
    }
  });

  // Get relay state
  server.on("/api/relay/state", HTTP_GET, []() {
    DynamicJsonDocument doc(64);
    doc["success"] = true;
    // doc["state"] = getRelayState();

    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
  });

  // Factory reset
  server.on("/api/factory-reset", HTTP_POST, []() {
    DynamicJsonDocument doc(128);
    doc["success"] = true;
    doc["message"] = "Factory reset initiated";

    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);

    delay(1000);
    factoryReset();
  });

  server.begin();
  Serial.println("✅ HTTP server started");
}
