


void initDoorSensor() {
  pinMode(DOOR_SENSOR_PIN, INPUT_PULLUP);
  Serial.println("Sensor pintu diinisialisasi di pin " + String(DOOR_SENSOR_PIN));
}

bool readDoorSensorRaw() {
  int sensorValue = digitalRead(DOOR_SENSOR_PIN);
  return (sensorValue == HIGH);  // true jika pintu terbuka
}

bool readDoorSensorDebounced() {
  bool rawState = readDoorSensorRaw();

  // Reset timer jika status berubah
  if (rawState != lastDoorStatus) {
    lastDebounceTime = millis();
  }

  // Cek jika sudah stabil selama debounce delay
  if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY) {
    if (rawState != doorStatus) {
      doorStatus = rawState;
      return doorStatus;
    }
  }

  lastDoorStatus = rawState;
  return doorStatus;
}

bool isDoorOpen() {
  return doorStatus;
}

bool isDoorClosed() {
  return !doorStatus;
}

String getDoorStatusText() {
  if (doorStatus) {
    return "TERBUKA";
  } else {
    return "TERTUTUP";
  }
}

int getDoorStatusNumeric() {
  return doorStatus ? 1 : 0;
}

bool doorAction() {
  switch (doorState) {
    case DOOR_CLOSED:
      if (isDoorOpen()) {
        Serial.println("Pintu mulai dibuka");
        doorState = DOOR_OPENING;
        return false;  // Relay ON
      }
      break;

    case DOOR_OPENING:
      if (isDoorOpen()) {
        Serial.println("Pintu terbuka penuh");
        doorState = DOOR_OPENED;
        return false;  // Relay masih ON
      }
      break;

    case DOOR_OPENED:
      if (isDoorClosed()) {
        Serial.println("Pintu mulai ditutup");
        doorState = DOOR_CLOSING;
        return false;  // Relay masih ON
      }
      break;

    case DOOR_CLOSING:
      if (isDoorClosed()) {

        Serial.println("Pintu tertutup penuh");
        doorState = DOOR_CLOSED;
        return true;  // Relay OFF
      }
      break;
  }

  return false;  // Default: relay tetap ON
}
void loopDoor() {
  static unsigned long prevMillis = 0;
  static bool doorState = false;
  unsigned long currentMillis = millis();
  readDoorSensorDebounced();
  if (doorAction()) {
    doorState = true;
    prevMillis = currentMillis;
  }
  if (doorState) {
    if (currentMillis - prevMillis > 1000) {
      AccessState = ACCESS_IDLE;
      beepRequest = 4;
      doorState = false;
    }
  }
}