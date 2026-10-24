#include <Wire.h>
#include <Si4703.h>
#include <U8g2lib.h>

#define USE_SERIAL   true
#define USE_OLED     true

// Full-buffer mode uses 1KB. Half-buffer uses 512 bytes.
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

Si4703 radio;

int channel = 9730;
int volume = 5;

void setup() {
  Serial.begin(9600);
  delay(500);

  radio.start();
  delay(100);
  radio.setVolExt(true);
  radio.setChannel(channel);
  radio.setVolume(volume);

  if (USE_OLED) {
    u8g2.begin();
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB10_tr);
  }
  
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
  u8g2.clearBuffer();

  int ch = radio.getChannel();
  int mhz = ch / 100;
  int khz = (ch % 100) / 10;

  u8g2.setFont(u8g2_font_ncenB10_tr);
  u8g2.setCursor(0, 16);
  u8g2.print(mhz);
  u8g2.print('.');
  u8g2.print(khz);
  u8g2.print(F("MHz"));

  u8g2.setFont(u8g2_font_ncenR08_tr);
  u8g2.setCursor(0, 34);
  u8g2.print(F("VOL:"));
  u8g2.print(radio.getVolume());
  u8g2.print(F(" RSSI:"));
  u8g2.print(radio.getRSSI());

  u8g2.setCursor(0, 48);
  u8g2.print(F("STEREO:"));
  u8g2.print(radio.getST() ? F("YES") : F("NO"));

  u8g2.sendBuffer();
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
