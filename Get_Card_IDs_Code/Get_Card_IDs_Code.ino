/*
  --------------------------------------------
  |                                          |
  |              Subscribe Now!!!            |
  |                                          |
  | http://www.youtube.com/@ChethanaVishmika |
  |                                          |
  --------------------------------------------

  CODE 1 - Get RFID card / tag IDs  (ESP32 + RC522 + buzzer)

  Open Serial Monitor at 115200 baud, tap a card, and copy the
  ready-made line it prints into CODE 2's student list.

  Library: "MFRC522" (Arduino Library Manager)

  Wiring (RC522 -> ESP32):
    SDA(SS) -> 5     SCK -> 18     MOSI -> 23
    MISO    -> 19    RST -> 22     GND  -> GND
    3.3V    -> 3V3 (NOT 5V!)       IRQ  -> not connected
  Buzzer: (+) -> GPIO 15, (-) -> GND
*/

#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN     5
#define RST_PIN    22
#define SCK_PIN    18
#define MISO_PIN   19
#define MOSI_PIN   23
#define BUZZER_PIN 15

MFRC522 rfid(SS_PIN, RST_PIN);

void beep(int duration = 150, int freq = 2700) {
  int halfPeriod = 500000 / freq;
  unsigned long end = millis() + duration;
  while (millis() < end) {
    digitalWrite(BUZZER_PIN, HIGH);
    delayMicroseconds(halfPeriod);
    digitalWrite(BUZZER_PIN, LOW);
    delayMicroseconds(halfPeriod);
  }
}

void setup() {
  Serial.begin(115200);
  delay(500);

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);
  rfid.PCD_Init();
  delay(50);

  Serial.println();
  Serial.println("=== CODE 1: Card ID Scanner ===");

  byte version = rfid.PCD_ReadRegister(MFRC522::VersionReg);
  Serial.print("RC522 firmware version: 0x");
  Serial.println(version, HEX);
  if (version == 0x00 || version == 0xFF) {
    Serial.println("WARNING: No communication with RC522! Check wiring/power (3.3V).");
  } else {
    Serial.println("RC522 detected OK.");
    beep(100);
  }
  Serial.println("Tap a card or tag...");
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent()) return;
  if (!rfid.PICC_ReadCardSerial()) return;

  // UID as plain HEX without colons, e.g. A1B2C3D4
  String uid = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) uid += "0";
    uid += String(rfid.uid.uidByte[i], HEX);
  }
  uid.toUpperCase();

  beep(200);

  Serial.println("--------------------------");
  Serial.print("Card ID : ");
  Serial.println(uid);
  Serial.println("Copy this line into CODE 2 (change the name):");
  Serial.print("  {\"");
  Serial.print(uid);
  Serial.println("\", \"Student Name\"},");

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
  delay(1000);
}
