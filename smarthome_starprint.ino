#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <EEPROM.h>
#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <HTTPUpdate.h>
#include <WebServer.h>
#include "env.c"
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 21
#define RST_PIN 22

MFRC522 mfrc522(SS_PIN, RST_PIN);
#define CONFIG_FILE "/config.json"
#define JSON_SIZE 2048

#define KEY_ADDR 500
#define KEY_LENGTH 40  // boleh 32 / 40 / 64


#define RELAY_PIN 14   // ESP32 pin GPIO21
#define BUZZER_PIN 13  // ESP32 pin GPIO22
#define DOOR_SENSOR_PIN 27
#define BUTTON_PIN 25
// Konfigurasi EEPROM
#define EEPROM_SIZE 64
#define RELAY_STATE_ADDR 0
#define WIFI_SSID_ADDR 10
#define WIFI_PASS_ADDR 40

bool buttonPressed = false;
unsigned long lastButtonPress = 0;
const unsigned long BUTTON_DEBOUNCE = 200;
String deviceKey = "";
String ap_name;
String ap_pass;
// Konfigurasi MQTT
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;
String deviceId = "";
WiFiClient espClient;
PubSubClient client(espClient);
WebServer server(80);

unsigned long lastDebounceTime = 0;
// bool relayState = false;
String ssid = "";
String password = "";
unsigned long lastMsgTime = 0;
unsigned long lastSensorRead = 0;
unsigned long apStartTime = 0;
bool inAPMode = false;

bool doorStatus = false;      // true = pintu terbuka
bool lastDoorStatus = false;  // status sebelumnya

const unsigned long DEBOUNCE_DELAY = 50;  // ms
bool CAN_TAP = false;
enum { DOOR_CLOSED,
       DOOR_OPENING,
       DOOR_OPENED,
       DOOR_CLOSING } doorState = DOOR_CLOSED;
enum RFIDMode {
  RFID_MODE_NORMAL,  // Mode normal - scan untuk akses
  RFID_MODE_ADD,     // Mode tambah - tunggu tap untuk menambah
  RFID_MODE_EDIT,    // Mode edit - tunggu tap untuk mengedit
  RFID_MODE_DELETE   // Mode delete - tunggu tap untuk menghapus
};

typedef enum {
  ACCESS_IDLE,     // belum ada akses
  ACCESS_GRANTED,  // akses diterima
  ACCESS_DENIED    // akses ditolak
} Access;
Access AccessState = ACCESS_IDLE;
int doorCount = 0;
int beepRequest = 0;
bool beepFinished = false;
String lastUID = "";
unsigned long lastReadTime = 0;

const unsigned long CARD_COOLDOWN = 5000;  // 2 detik
RFIDMode currentRFIDMode = RFID_MODE_NORMAL;
String pendingUID = "";   // UID yang sedang ditunggu untuk operasi
String pendingName = "";  // Nama untuk operasi ADD/EDIT
unsigned long modeTimeout = 0;
const unsigned long MODE_TIMEOUT_MS = 30000;  // 30 detik timeout
// Function prototypes
// void initHardware();
void initEEPROM();
// void loadRelayState();
void generateDeviceId();
void setupMQTT();
void setupOTA();
void checkForOTAUpdate();
void handleMQTTConnection();
void saveWiFiCredentials(String ssid, String pass);
void loadWiFiCredentials();
void startAPMode();
void setup() {
  Serial.begin(115200);
  delay(1000);
  initLittleFS();
  initButton();
  // initEEPROM();
  generateDeviceId();
  loadWiFiCredentials();
  setupVariableFromLittleFS();
  initRelay();
  initBuzzer();
  initRFID();
  initDoorSensor();
  // saveDefaultConfig();
  if (ssid.length() > 0) {
    Serial.println("Connecting to saved WiFi...");
    WiFi.begin(ssid.c_str(), password.c_str());

    unsigned long startTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startTime < 10000) {
      delay(500);
      Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\nWiFi connected!");
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
      generateDeviceId();

      // Hanya setup MQTT dan OTA, TIDAK memulai web server
      setupMQTT();
      setupOTA();
      checkForOTAUpdate();
      currentRFIDMode = RFID_MODE_NORMAL;
      pendingUID = "";
      pendingName = "";
      // Stop AP mode jika sedang aktif
      if (inAPMode) {
        WiFi.mode(WIFI_STA);
        inAPMode = false;
        Serial.println("AP mode stopped");
      }
      return;
    }
  }

  // Jika tidak ada WiFi yang tersimpan atau koneksi gagal, start AP mode
  startAPMode();
}

void loop() {
  if (inAPMode) {
    server.handleClient();
loopCard();
    loopBuzzer();
    loopDoor();
    relayLoop();
    loopButton();
    // Auto-exit AP mode after 10 minutes
    if (millis() - apStartTime > 600000) {
      Serial.println("AP mode timeout, restarting...");
      ESP.restart();
    }
  } else {
    // Mode STA - hanya handle MQTT dan OTA
    handleMQTTConnection();
    client.loop();
    ArduinoOTA.handle();
        loopButton();
    loopCard();
    loopBuzzer();
    loopDoor();
    relayLoop();
    // Tidak ada web server di mode STA
  }
}

void loadWiFiCredentials() {

  ssid = getConfigString("wifi_ssid");

  password = getConfigString("wifi_pass");


  Serial.print("Loaded SSID: ");
  Serial.println(ssid);
  Serial.print("Loaded Password: ");
  Serial.println(password);
}

void saveWiFiCredentials(String ssid, String pass) {
  EEPROM.begin(EEPROM_SIZE);

  // Save SSID
  for (int i = 0; i < 30; i++) {
    if (i < ssid.length()) {
      EEPROM.write(WIFI_SSID_ADDR + i, ssid[i]);
    } else {
      EEPROM.write(WIFI_SSID_ADDR + i, 0);
    }
  }

  // Save Password
  for (int i = 0; i < 30; i++) {
    if (i < pass.length()) {
      EEPROM.write(WIFI_PASS_ADDR + i, pass[i]);
    } else {
      EEPROM.write(WIFI_PASS_ADDR + i, 0);
    }
  }

  EEPROM.commit();
  EEPROM.end();

  Serial.println("WiFi credentials saved");
}
