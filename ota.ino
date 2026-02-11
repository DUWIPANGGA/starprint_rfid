  void setupOTA() {
    // Hostname default adalah esp8266-[ChipID]
    ArduinoOTA.setHostname(("SmartPlug-" + WiFi.macAddress().substring(12,17)).c_str());

    // Password OTA (opsional)
    // ArduinoOTA.setPassword("admin123");

    ArduinoOTA.onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH) {
        type = "sketch";
      } else { // U_SPIFFS
        type = "filesystem";
      }
      Serial.println("Start updating " + type);
    });
    
    ArduinoOTA.onEnd([]() {
      Serial.println("\nEnd");
    });
    
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    
    ArduinoOTA.onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });
    
    ArduinoOTA.begin();
    Serial.println("OTA Ready");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }

  String getLatestVersion() {
    WiFiClientSecure secureClient;
    secureClient.setInsecure();
    
    HTTPClient http;
    
    Serial.print("Fetching version from: ");
    Serial.println(VERSION_URL);
    
    if (!http.begin(secureClient, VERSION_URL)) {
      Serial.println("Failed to begin HTTP connection");
      return "";
    }

    http.setTimeout(10000);
    
    int httpCode = http.GET();
    
    if (httpCode != HTTP_CODE_OK) {
      Serial.printf("HTTP error code: %d\n", httpCode);
      http.end();
      return "";
    }

    String payload = http.getString();
    payload.trim(); // Memanggil trim() sebagai statement terpisah
    http.end();
    
    Serial.print("Latest version: ");
    Serial.println(payload);
    
    return payload;
  }

  bool shouldUpdate(String currentVersion, String latestVersion) {
    if (latestVersion.isEmpty()) {
      Serial.println("Latest version string is empty");
      return false;
    }

    // Hitung jumlah titik sebagai pengganti count()
    int dotCount = 0;
    for (unsigned int i = 0; i < latestVersion.length(); i++) {
      if (latestVersion.charAt(i) == '.') dotCount++;
    }
    
    if (latestVersion.length() > 10 || dotCount != 2) {
      Serial.println("Invalid version format");
      return false;
    }

    int currentParts[3] = {0};
    int latestParts[3] = {0};
    
    if (sscanf(currentVersion.c_str(), "%d.%d.%d", &currentParts[0], &currentParts[1], &currentParts[2]) != 3) {
      Serial.println("Failed to parse current version");
      return false;
    }
    
    if (sscanf(latestVersion.c_str(), "%d.%d.%d", &latestParts[0], &latestParts[1], &latestParts[2]) != 3) {
      Serial.println("Failed to parse latest version");
      return false;
    }

    for (int i = 0; i < 3; i++) {
      if (latestParts[i] > currentParts[i]) {
        Serial.println("New update available");
        return true;
      }
      if (latestParts[i] < currentParts[i]) {
        Serial.println("Current version is newer");
        return false;
      }
    }
    
    Serial.println("Versions are identical");
    return false;
  }

  void checkForOTAUpdate() {
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("WiFi not connected!");
      return;
    }

    Serial.println("Checking for updates...");
    
    String latestVersion = getLatestVersion();
    if (latestVersion.isEmpty()) {
      Serial.println("Failed to get latest version");
      return;
    }

    if (!shouldUpdate(CURRENT_VERSION, latestVersion)) {
      return;
    }

    WiFiClientSecure client;
    client.setInsecure();
    
    // httpUpdate.setLedPin(LED_BUILTIN, LOW);
    httpUpdate.rebootOnUpdate(true);
    
    // Alternatif untuk setUserAgent yang tidak tersedia
    // Tambahkan header User-Agent melalui HTTPClient jika perlu
    
    Serial.print("Downloading firmware from: ");
    Serial.println(FIRMWARE_URL);

    for (int attempt = 1; attempt <= 3; attempt++) {
      Serial.printf("Attempt %d/3...\n", attempt);
      
      t_httpUpdate_return ret = httpUpdate.update(client, FIRMWARE_URL);

      switch (ret) {
        case HTTP_UPDATE_OK:
          Serial.println("Update successful!");
          return;
          
        case HTTP_UPDATE_NO_UPDATES:
          Serial.println("No updates available.");
          return;
          
        case HTTP_UPDATE_FAILED:
          Serial.printf("Update failed (attempt %d): %s\n", 
                      attempt, 
                      httpUpdate.getLastErrorString().c_str());
          if (attempt < 3) delay(5000);
          break;
      }
    }
    
    Serial.println("All update attempts failed");
  }