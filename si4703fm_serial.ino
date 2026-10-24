#include <Si4703.h>
#include <Wire.h>

Si4703 radio;

int channel = 9370;        // 93.00 MHz (×100 format)
int volume = 2;

void setup() {
  Serial.begin(9600);

  radio.start();
  radio.setChannel(channel);
  radio.setVolume(volume);

  Serial.println("\n\nSi4703 FM Radio Test Sketch");
  Serial.println("=============================");
  Serial.println("a b     Favourite stations");
  Serial.println("+ -     Volume (max 15)");
  Serial.println("n l     Seek up / down");
  Serial.println("r       Read RDS Data");
  Serial.println("i       Full status info");
  Serial.println("e       Toggle Extended Volume (VOLEXT)");
  Serial.println("m       Toggle Mute");
  Serial.println("s       Toggle Mono/Stereo");
  Serial.println("Send me a command letter.");
}

void loop() {
  if (Serial.available()) {
    char ch = Serial.read();

    if (ch == 'n') {
      int newCh = radio.seekUp();
      if (newCh == 0)
        Serial.println("Seek failed or band limit reached");
      else {
        channel = newCh;
        displayInfo();
      }
    }
    else if (ch == 'l') {
      int newCh = radio.seekDown();
      if (newCh == 0)
        Serial.println("Seek failed or band limit reached");
      else {
        channel = newCh;
        displayInfo();
      }
    }
    else if (ch == '+') {
      radio.incVolume();
      displayInfo();
    }
    else if (ch == '-') {
      radio.decVolume();
      displayInfo();
    }
    else if (ch == 'a') {
      channel = 9930;
      radio.setChannel(channel);
      displayInfo();
    }
    else if (ch == 'b') {
      channel = 10150;
      radio.setChannel(channel);
      displayInfo();
    }
    else if (ch == 'r') {
      Serial.println("Reading RDS...");
      readRDSInfo();
    }
    else if (ch == 'i') {
      Serial.println("=== Full Status Info ===");
      fullInfo();
    }
    else if (ch == 'e') {
      bool current = radio.getVolExt();
      radio.setVolExt(!current);
      Serial.println(radio.getVolExt() ? "--- VOLEXT ON (extended curve) ---" : "--- VOLEXT OFF (standard curve) ---");
      Serial.println("Try volume 1-5 to hear the difference at low levels.");
      printState();
    }
    else if (ch == 'm') {
      bool current = radio.getMute();
      radio.setMute(!current);
      Serial.print("Audio: ");
      Serial.println(radio.getMute() ? "ON" : "MUTED");
    }
    else if (ch == 's') {
      bool current = radio.getMono();
      radio.setMono(!current);
      Serial.print("Mono mode: ");
      Serial.println(radio.getMono() ? "FORCED MONO" : "AUTO STEREO");
    }
  }
}

void displayInfo() {
  Serial.print("Ch: ");
  Serial.print(float(radio.getChannel()) / 100.0, 2);
  Serial.print(" MHz | VOL: ");
  Serial.print(radio.getVolume());
  Serial.print(" | ");
  Serial.print(radio.getST() ? "STEREO" : "MONO  ");
  Serial.print(" | RSSI: ");
  Serial.print(radio.getRSSI());
  Serial.print(" | Audio: ");
  Serial.println(radio.getMute() ? "ON" : "MUTED");
  printState();
}

void readRDSInfo() {
  radio.readRDS();
  Serial.println("=== RDS Raw Data ===");
  Serial.println("radio.readRDS() called to refresh internal registers.");
  Serial.println("NOTE: mkhuthir library does NOT expose decoded RDS text.");
  Serial.println("For actual RDS strings, use SparkFunSi4703rds library instead.");
}

void fullInfo() {
  Serial.print("Part Number:    0x"); Serial.println(radio.getPN(), HEX);
  Serial.print("Mfg ID:         0x"); Serial.println(radio.getMFGID(), HEX);
  Serial.print("Device:         0x"); Serial.println(radio.getDEV(), HEX);
  Serial.print("Chip Rev:       0x"); Serial.println(radio.getREV(), HEX);
  Serial.print("Firmware:       0x"); Serial.println(radio.getFIRMWARE(), HEX);
  Serial.print("Band Start:     "); Serial.print(float(radio.getBandStart())/100.0, 2); Serial.println(" MHz");
  Serial.print("Band End:       "); Serial.print(float(radio.getBandEnd())/100.0, 2); Serial.println(" MHz");
  Serial.print("Band Space:     "); Serial.print(float(radio.getBandSpace())/100.0, 2); Serial.println(" MHz");
  Serial.print("Current Ch:     "); Serial.print(float(radio.getChannel())/100.0, 2); Serial.println(" MHz");
  Serial.print("Volume:         "); Serial.println(radio.getVolume());
  Serial.print("RSSI:           "); Serial.println(radio.getRSSI());
  Serial.print("Stereo:         "); Serial.println(radio.getST() ? "Yes" : "No");
  Serial.print("Mono Mode:      "); Serial.println(radio.getMono() ? "Forced" : "Auto");
  Serial.print("Audio:          "); Serial.println(radio.getMute() ? "ON (unmuted)" : "OFF (muted)");
  Serial.print("Vol Extended:   "); Serial.println(radio.getVolExt() ? "ON (alt curve)" : "OFF (standard)");
}

void printState() {
  Serial.print(">>> VOL=");
  Serial.print(radio.getVolume());
  Serial.print("  VOLEXT=");
  Serial.print(radio.getVolExt() ? "ON " : "OFF");
  Serial.print("  Ch=");
  Serial.print(float(radio.getChannel())/100.0, 2);
  Serial.println(" MHz");
}
