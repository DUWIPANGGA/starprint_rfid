void startAPMode() {
  inAPMode = true;
  apStartTime = millis();

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ap_name, ap_pass);

  Serial.println("AP Mode Started");
  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());

  setupWebServer(); 
}