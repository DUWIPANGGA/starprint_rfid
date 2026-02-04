
void initBuzzer() {
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
}
void beepSingleTit() {
  static byte state = 0;
  static unsigned long prevMillis = 0;

  unsigned long now = millis();
  beepFinished = false;

  switch (state) {
    case 0: // Start beep
      digitalWrite(BUZZER_PIN, HIGH);
      prevMillis = now;
      state = 1;
      break;

    case 1: // Beep duration
      if (now - prevMillis >= 100) {
        digitalWrite(BUZZER_PIN, LOW);
        state = 0;
        beepFinished = true; // ✅ selesai
      }
      break;
  }
}

void beepDoubleTit() {
  static byte state = 0;
  static unsigned long prevMillis = 0;

  unsigned long now = millis();
  beepFinished = false;

  switch (state) {
    case 0:
      digitalWrite(BUZZER_PIN, HIGH);
      prevMillis = now;
      state = 1;
      break;

    case 1:
      if (now - prevMillis >= 100) {
        digitalWrite(BUZZER_PIN, LOW);
        prevMillis = now;
        state = 2;
      }
      break;

    case 2:
      if (now - prevMillis >= 100) {
        digitalWrite(BUZZER_PIN, HIGH);
        prevMillis = now;
        state = 3;
      }
      break;

    case 3:
      if (now - prevMillis >= 100) {
        digitalWrite(BUZZER_PIN, LOW);
        state = 0;
        beepFinished = true;  // ✅ selesai
      }
      break;
  }
}

bool beepDoubleStateFinished() {
  return beepFinished;
}

void beepTripleTit() {
  static byte state = 0;
  static unsigned long prevMillis = 0;

  unsigned long now = millis();
  beepFinished = false; // ⬅️ penting

  switch (state) {
    case 0:  // Beep 1 ON
      digitalWrite(BUZZER_PIN, HIGH);
      prevMillis = now;
      state = 1;
      break;

    case 1:  // Beep 1 OFF
      if (now - prevMillis >= 100) {
        digitalWrite(BUZZER_PIN, LOW);
        prevMillis = now;
        state = 2;
      }
      break;

    case 2:  // Jeda 1
      if (now - prevMillis >= 100) {
        digitalWrite(BUZZER_PIN, HIGH);
        prevMillis = now;
        state = 3;
      }
      break;

    case 3:  // Beep 2 OFF
      if (now - prevMillis >= 100) {
        digitalWrite(BUZZER_PIN, LOW);
        prevMillis = now;
        state = 4;
      }
      break;

    case 4:  // Jeda 2
      if (now - prevMillis >= 100) {
        digitalWrite(BUZZER_PIN, HIGH);
        prevMillis = now;
        state = 5;
      }
      break;

    case 5:  // Beep 3 OFF
      if (now - prevMillis >= 100) {
        digitalWrite(BUZZER_PIN, LOW);
        state = 0;            // reset FSM
        beepFinished = true;  // ✅ selesai
      }
      break;
  }
}
void beepLongTit() {
  static byte state = 0;
  static unsigned long prevMillis = 0;

  unsigned long now = millis();
  beepFinished = false;  // ⬅️ wajib

  switch (state) {
    case 0:  // Start beep
      digitalWrite(BUZZER_PIN, HIGH);
      prevMillis = now;
      state = 1;
      break;

    case 1:  // Bunyi panjang (500 ms)
      if (now - prevMillis >= 500) {
        digitalWrite(BUZZER_PIN, LOW);
        prevMillis = now;
        state = 2;
      }
      break;

    case 2:  // Jeda akhir (200 ms)
      if (now - prevMillis >= 200) {
        state = 0;            // reset FSM
        beepFinished = true;  // ✅ selesai
      }
      break;
  }
}

// Fungsi untuk pola lengkap: tit tit, tit tit tit, tit panjang
void beepCompletePattern() {
  static unsigned long patternMillis = 0;
  static uint8_t patternState = 0;  // 0: idle, 1-2: tit tit, 3: jeda, 4-6: tit tit tit, 7: jeda, 8: tit panjang

  unsigned long currentMillis = millis();

  switch (patternState) {
    case 0:  // Mulai pola
      patternMillis = currentMillis;
      patternState = 1;
      // Tidak ada break, langsung ke case 1

    case 1:  // Tit pertama dari pasangan pertama
      if (currentMillis - patternMillis >= 0) {
        digitalWrite(BUZZER_PIN, HIGH);
        patternMillis = currentMillis;
        patternState = 2;
      }
      break;

    case 2:  // Tit kedua dari pasangan pertama
      if (currentMillis - patternMillis >= 100) {
        digitalWrite(BUZZER_PIN, LOW);
        patternMillis = currentMillis;
        patternState = 3;
      }
      break;

    case 3:  // Jeda setelah tit tit
      if (currentMillis - patternMillis >= 200) {
        // Mulai triple tit
        digitalWrite(BUZZER_PIN, HIGH);
        patternMillis = currentMillis;
        patternState = 4;
      }
      break;

    case 4:  // Tit pertama dari triple
      if (currentMillis - patternMillis >= 100) {
        digitalWrite(BUZZER_PIN, LOW);
        patternMillis = currentMillis;
        patternState = 5;
      }
      break;

    case 5:  // Jeda setelah tit pertama
      if (currentMillis - patternMillis >= 50) {
        digitalWrite(BUZZER_PIN, HIGH);
        patternMillis = currentMillis;
        patternState = 6;
      }
      break;

    case 6:  // Tit kedua dari triple
      if (currentMillis - patternMillis >= 100) {
        digitalWrite(BUZZER_PIN, LOW);
        patternMillis = currentMillis;
        patternState = 7;
      }
      break;

    case 7:  // Jeda setelah tit kedua
      if (currentMillis - patternMillis >= 50) {
        digitalWrite(BUZZER_PIN, HIGH);
        patternMillis = currentMillis;
        patternState = 8;
      }
      break;

    case 8:  // Tit ketiga dari triple
      if (currentMillis - patternMillis >= 100) {
        digitalWrite(BUZZER_PIN, LOW);
        patternMillis = currentMillis;
        patternState = 9;
      }
      break;

    case 9:  // Jeda sebelum tit panjang
      if (currentMillis - patternMillis >= 200) {
        digitalWrite(BUZZER_PIN, HIGH);
        patternMillis = currentMillis;
        patternState = 10;
      }
      break;

    case 10:  // Tit panjang
      if (currentMillis - patternMillis >= 500) {
        digitalWrite(BUZZER_PIN, LOW);
        patternState = 0;  // Reset pola
      }
      break;
  }
}

// Fungsi wrapper untuk memudahkan penggunaan
void beep(uint8_t pattern) {
  switch (pattern) {
    case 1:
      beepSingleTit();
      break;
    case 2:
      beepDoubleTit();
      break;
    case 3:
      beepTripleTit();
      break;
    case 4:
      beepLongTit();
      break;
    case 5:
      beepCompletePattern();
      break;
  }
}
void loopBuzzer() {
  if (beepRequest>0) {
    switch(beepRequest){
      case 0:
      return;
      break;
      case 1:
      beepDoubleTit();
      break;
      case 2:
      beepLongTit();
      break;
      case 3:
      beepTripleTit();
      break;
      case 4:
      beepSingleTit();
      break;
    }
    
    if (beepDoubleStateFinished()) {
      beepRequest = 0;
    }
  }
  return;
}
void sendBeepRequest3(){
  beepRequest = 3;
}