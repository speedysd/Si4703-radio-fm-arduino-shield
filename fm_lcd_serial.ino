#include <Wire.h>
#include <Si4703.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define USE_SERIAL   true
#define USE_OLED     true

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

int freeRam() {
  extern int __heap_start, *__brkval;
  int v;
  return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
}

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

Si4703 radio;

int channel = 9730;        // 97.30 MHz default
int volume = 5;

void setup() {
  Serial.begin(9600);
  delay(500);

  // --- Radio SECOND (its internal Wire.begin() is now a no-op) ---
  radio.start();
  delay(100);
  radio.setVolExt(true);
  radio.setChannel(channel);
  radio.setVolume(volume);

  // --- OLED FIRST (it calls Wire.begin() internally) ---
  if (USE_OLED) {
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
      Serial.println(F("SSD1306 failed."));
    } else {
      display.clearDisplay();
      display.setTextColor(SSD1306_WHITE);
    }
  }

  if (USE_SERIAL) printSerialHelp();
  if (USE_OLED) updateOLED();
}


void setup2() {
  Serial.begin(9600);
  delay(500);

  // --- OLED FIRST (it calls Wire.begin() internally) ---
  if (USE_OLED) {
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
      Serial.println(F("SSD1306 failed."));
    } else {
      display.clearDisplay();
      display.setTextColor(SSD1306_WHITE);
    }
  }

  // --- Radio SECOND (its internal Wire.begin() is now a no-op) ---
  radio.start();
  delay(100);
  radio.setVolExt(true);
  radio.setChannel(channel);
  radio.setVolume(volume);

  if (USE_SERIAL) printSerialHelp();
  if (USE_OLED) updateOLED();
}

void setup1() {
  Serial.begin(9600);
  delay(500);

//  Wire.begin();
//  delay(200);

  Serial.print(F("Free RAM before OLED init: "));
  Serial.println(freeRam());

  if (USE_OLED) {
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
      Serial.print(F("SSD1306 failed. Free RAM: "));
      Serial.println(freeRam());
    } else {
      Serial.print(F("OLED OK. Free RAM after: "));
      Serial.println(freeRam());
      display.clearDisplay();
      display.setTextColor(SSD1306_WHITE);
    }
  }

  radio.start();
  delay(100);

  // Extended volume ON by default
  radio.setVolExt(true);

  radio.setChannel(channel);
  radio.setVolume(volume);

  Serial.print(F("Radio OK. Free RAM: "));
  Serial.println(freeRam());

  if (USE_SERIAL) printSerialHelp();
  if (USE_OLED) updateOLED();
}

void loop() {
  if (Serial.available()) {
    char ch = Serial.read();

    if (ch == 'n') {
      int newCh = radio.seekUp();
      if (newCh != 0) {
        channel = newCh;
        if (USE_SERIAL) Serial.println(F("Seeked up"));
        if (USE_OLED) updateOLED();
      } else {
        if (USE_SERIAL) Serial.println(F("Seek failed"));
      }
    }
    else if (ch == 'l') {
      int newCh = radio.seekDown();
      if (newCh != 0) {
        channel = newCh;
        if (USE_SERIAL) Serial.println(F("Seeked down"));
        if (USE_OLED) updateOLED();
      } else {
        if (USE_SERIAL) Serial.println(F("Seek failed"));
      }
    }
    else if (ch == '+') {
      radio.incVolume();
      if (USE_SERIAL) Serial.println(F("Volume up"));
      if (USE_OLED) updateOLED();
    }
    else if (ch == '-') {
      radio.decVolume();
      if (USE_SERIAL) Serial.println(F("Volume down"));
      if (USE_OLED) updateOLED();
    }
    else if (ch == 'a') {
      radio.setChannel(9300);
      if (USE_SERIAL) Serial.println(F("Tuned to 93.0 MHz"));
      if (USE_OLED) updateOLED();
    }
    else if (ch == 'b') {
      radio.setChannel(9740);
      if (USE_SERIAL) Serial.println(F("Tuned to 97.4 MHz"));
      if (USE_OLED) updateOLED();
    }
    else if (ch == 'u') {
      channel = radio.getChannel() + 10;
      if (channel > 10800) channel = 8750;
      radio.setChannel(channel);
      Serial.print(F("Ch: "));
      Serial.print(float(radio.getChannel())/100.0, 2);
      Serial.print(F(" RSSI: "));
      Serial.println(radio.getRSSI());
      if (USE_OLED) updateOLED();
    }
    else if (ch == 'd') {
      channel = radio.getChannel() - 10;
      if (channel < 8750) channel = 10800;
      radio.setChannel(channel);
      Serial.print(F("Ch: "));
      Serial.print(float(radio.getChannel())/100.0, 2);
      Serial.print(F(" RSSI: "));
      Serial.println(radio.getRSSI());
      if (USE_OLED) updateOLED();
    }
    else if (ch == 'e') {
      bool cur = radio.getVolExt();
      radio.setVolExt(!cur);
      Serial.println(radio.getVolExt() ? F("--- VOLEXT ON ---") : F("--- VOLEXT OFF ---"));
    }
    else if (ch == 'm') {
      bool cur = radio.getMute();
      radio.setMute(!cur);
      Serial.print(F("Audio: "));
      Serial.println(radio.getMute() ? F("ON") : F("MUTED"));
    }
    else if (ch == 'i') {
      printFullInfo();
    }
    else if (ch == 's') {
      printSerialStatus();
    }
    else if (ch == 'h') {
      if (USE_SERIAL) printSerialHelp();
    }
    else if (ch != '\r' && ch != '\n') {
      if (USE_SERIAL) {
        Serial.print(F("Unknown: '"));
        Serial.print(ch);
        Serial.println(F("'"));
        printSerialHelp();
      }
    }
  }
}

void updateOLED() {
  display.clearDisplay();

  int ch = radio.getChannel();
  int mhz = ch / 100;
  int khz = (ch % 100) / 10;

  display.setTextSize(2);
  display.setCursor(0, 0);
  display.print(mhz);
  display.print('.');
  display.print(khz);
  display.println(F("MHz"));

  display.setTextSize(1);
  display.setCursor(0, 30);
  display.print(F("VOL:"));
  display.print(radio.getVolume());
  display.print(F(" RSSI:"));
  display.print(radio.getRSSI());

  display.setCursor(0, 42);
  display.print(F("STEREO:"));
  display.println(radio.getST() ? F("YES") : F("NO"));

  display.display();
}

void printSerialStatus() {
  Serial.println(F("\n=== RADIO STATUS ==="));
  Serial.print(F("Channel: "));
  Serial.print(float(radio.getChannel()) / 100.0, 2);
  Serial.println(F(" MHz"));
  Serial.print(F("Volume: "));
  Serial.println(radio.getVolume());
  Serial.print(F("RSSI: "));
  Serial.println(radio.getRSSI());
  Serial.print(F("Stereo: "));
  Serial.println(radio.getST() ? F("Yes") : F("No"));
  Serial.print(F("Audio: "));
  Serial.println(radio.getMute() ? F("ON") : F("MUTED"));
  Serial.print(F("VOLEXT: "));
  Serial.println(radio.getVolExt() ? F("ON") : F("OFF"));
  Serial.print(F("Free RAM: "));
  Serial.println(freeRam());
  Serial.println(F("====================\n"));
}

void printFullInfo() {
  Serial.println(F("\n=== Full Status Info ==="));
  Serial.print(F("Part Number:    0x")); Serial.println(radio.getPN(), HEX);
  Serial.print(F("Mfg ID:         0x")); Serial.println(radio.getMFGID(), HEX);
  Serial.print(F("Device:         0x")); Serial.println(radio.getDEV(), HEX);
  Serial.print(F("Chip Rev:       0x")); Serial.println(radio.getREV(), HEX);
  Serial.print(F("Firmware:       0x")); Serial.println(radio.getFIRMWARE(), HEX);
  Serial.print(F("Band Start:     ")); Serial.print(float(radio.getBandStart())/100.0, 2); Serial.println(F(" MHz"));
  Serial.print(F("Band End:       ")); Serial.print(float(radio.getBandEnd())/100.0, 2); Serial.println(F(" MHz"));
  Serial.print(F("Band Space:     ")); Serial.print(float(radio.getBandSpace())/100.0, 2); Serial.println(F(" MHz"));
  Serial.print(F("Current Ch:     ")); Serial.print(float(radio.getChannel())/100.0, 2); Serial.println(F(" MHz"));
  Serial.print(F("Volume:         ")); Serial.println(radio.getVolume());
  Serial.print(F("RSSI:           ")); Serial.println(radio.getRSSI());
  Serial.print(F("Stereo:         ")); Serial.println(radio.getST() ? F("Yes") : F("No"));
  Serial.print(F("Mono Mode:      ")); Serial.println(radio.getMono() ? F("Forced") : F("Auto"));
  Serial.print(F("Audio:          ")); Serial.println(radio.getMute() ? F("ON (unmuted)") : F("OFF (muted)"));
  Serial.print(F("Vol Extended:   ")); Serial.println(radio.getVolExt() ? F("ON (alt curve)") : F("OFF (standard)"));
  Serial.print(F("Free RAM:       ")); Serial.println(freeRam());
  Serial.println(F("========================\n"));
}

void printSerialHelp() {
  Serial.println(F("\na b  Favourite stations"));
  Serial.println(F("+ -  Volume +/-"));
  Serial.println(F("n l  Seek Up/Down"));
  Serial.println(F("u d  Tune up/down 0.1 MHz"));
  Serial.println(F("e    Toggle Extended Volume"));
  Serial.println(F("m    Toggle Mute"));
  Serial.println(F("i    Full status info"));
  Serial.println(F("s    Print status"));
  Serial.println(F("h    Help\n"));
}
