void initRelay() {
  pinMode(RELAY_PIN, OUTPUT);
  relayOff();
}

// Fungsi untuk menyalakan relay
void relayOn() {
  digitalWrite(RELAY_PIN, LOW);
}

// Fungsi untuk mematikan relay
void relayOff() {
  digitalWrite(RELAY_PIN, HIGH);
}

// Fungsi untuk toggle relay
void relayToggle() {
  digitalWrite(RELAY_PIN, !digitalRead(RELAY_PIN));
}

// Fungsi untuk mengecek status relay
bool isRelayOn() {
  return digitalRead(RELAY_PIN) == HIGH;
}
void relayState(Access state) {
  switch (state) {
    case ACCESS_IDLE:
      relayOff();
      break;
    case ACCESS_GRANTED:
      relayOn();
      break;
    case ACCESS_DENIED:
      relayOff();
      break;
    default:
      relayOff();
      break;
  }
}
void relayLoop() {
  static unsigned long prevMillis = 0;
  static unsigned long doorOpenMillis = 0;
  static bool doorTimerActive = false;
  static bool prevUpdate = false;
  unsigned long now = millis();
  relayState(AccessState);
  if (AccessState == ACCESS_GRANTED && doorState == DOOR_CLOSED) {
    if (!prevUpdate) {
      prevMillis = now;
      prevUpdate = true;
    }
    if (now - prevMillis > 10000) {
      AccessState = ACCESS_IDLE;
      Serial.println("set access to idle");
      prevUpdate = false;
      beepRequest = 4;
    }
    doorTimerActive = false;
    CAN_TAP = false;
  }




  else if (AccessState == ACCESS_GRANTED && doorState == DOOR_OPENED) {

    if (!doorTimerActive) {
      doorOpenMillis = now;
      prevMillis = now;
      doorTimerActive = true;
    }
    if (now - doorOpenMillis > 30000) {
      if (!prevUpdate) {
        prevMillis = now;
        prevUpdate = true;
      }
      if (now - prevMillis > 3000) {
        beepRequest = 3;
        Serial.println("set access to idle");
        prevUpdate = false;
      }
    }
    CAN_TAP = false;

  }
else if (AccessState == ACCESS_IDLE && doorState == DOOR_CLOSED) {
    CAN_TAP = true;
}
else if (AccessState == ACCESS_IDLE && doorState == DOOR_CLOSED) {
    CAN_TAP = true;
}



  else {
    // doorTimerActive = false;
  }
}