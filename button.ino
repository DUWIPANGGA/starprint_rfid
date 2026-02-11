void initButton() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void loopButton() {
  static bool buttonHolding = false;
  static unsigned long holdStart = 0;
  static bool longPressTriggered = false;

  bool pressed = digitalRead(BUTTON_PIN) == LOW;

  if (pressed && !buttonHolding) {
    buttonHolding = true;
    holdStart = millis();
    longPressTriggered = false;
  }

  if (pressed && buttonHolding) {
    if (!longPressTriggered && millis() - holdStart >= 10000) { // 10 DETIK
      longPressTriggered = true;

      Serial.println("🔥 LONG PRESS 10 DETIK");

      beepRequest = 3;
      resetDeviceConfig();
      ESP.restart();
    }
  }

  if (!pressed && buttonHolding) {
    unsigned long pressDuration = millis() - holdStart;

    if (pressDuration < 10000 && !longPressTriggered) {
      Serial.println("BUTTON DITEKAN");

      if (AccessState == ACCESS_IDLE) {
        AccessState = ACCESS_GRANTED;
        beepRequest = 1;
      }
    }

    buttonHolding = false;
  }
}