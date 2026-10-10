/*
  --------------------------------------------
  |                                          |
  |              Subscribe Now!!!            |
  |                                          |
  | http://www.youtube.com/@ChethanaVishmika |
  |                                          |
  --------------------------------------------

  CODE 2 - RFID Attendance System  (ESP32 + RC522 + buzzer + Google Sheets)

  - Scan a registered card  -> 1 beep, attendance row added to Google Sheet
  - Scan an unregistered card -> 2 low beeps, nothing is saved
  - Upload to Google Sheets failed (WiFi/URL problem) -> 3 quick beeps

  The Date and Time are added by Google Apps Script (Code.gs), so the
  ESP32 only sends the student's name.

  Library: "MFRC522" (Arduino Library Manager). WiFi/HTTPClient are built in.

  Wiring (RC522 -> ESP32):
    SDA(SS) -> 5     SCK -> 18     MOSI -> 23
    MISO    -> 19    RST -> 22     GND  -> GND
    3.3V    -> 3V3 (NOT 5V!)       IRQ  -> not connected
  Buzzer: (+) -> GPIO 15, (-) -> GND
*/

#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

// ================== EDIT THESE ==================
const char* WIFI_SSID  = "YOUR_WIFI_NAME";
const char* WIFI_PASS  = "YOUR_WIFI_PASSWORD";

// Paste your Google Apps Script "Web app" URL here (ends with /exec)
const char* SCRIPT_URL = "https://script.google.com/macros/s/PASTE_YOUR_DEPLOYMENT_ID_HERE/exec";

// Registered cards: { "CARD_ID", "Student Name" }
// Get the card IDs using CODE 1.
struct Student {
  const char* uid;
  const char* name;
};

Student students[] = {
  {"ID 1", "Student Name 1"},
  {"ID 2", "Student Name 2"},
  {"ID 3", "Student Name 3"},
  // add more cards here...
};

const unsigned long COOLDOWN_MS = 10000;  // ignore the same card for 10 seconds
// =================================================

#define SS_PIN     5
#define RST_PIN    22
#define SCK_PIN    18
#define MISO_PIN   19
#define MOSI_PIN   23
#define BUZZER_PIN 15

const int STUDENT_COUNT = sizeof(students) / sizeof(students[0]);

MFRC522 rfid(SS_PIN, RST_PIN);

String lastUid = "";
unsigned long lastScanTime = 0;

// ---------- Buzzer ----------
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

void beepValid()   { beep(200); }
void beepUnknown() { beep(250, 800); delay(100); beep(250, 800); }
void beepError()   { for (int i = 0; i < 3; i++) { beep(80); delay(80); } }

// ---------- WiFi ----------
bool connectWiFi() {
  if (WiFi.status() == WL_CONNECTED) return true;

  Serial.print("Connecting to WiFi");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 15000) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("WiFi connected. IP: ");
    Serial.println(WiFi.localIP());
    return true;
  }
  Serial.println("WiFi connection failed.");
  return false;
}

// ---------- Helpers ----------
String urlEncode(const String& s) {
  String out = "";
  char buf[4];
  for (size_t i = 0; i < s.length(); i++) {
    char c = s[i];
    if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
      out += c;
    } else {
      sprintf(buf, "%%%02X", (uint8_t)c);
      out += buf;
    }
  }
  return out;
}

int findStudent(const String& uid) {
  for (int i = 0; i < STUDENT_COUNT; i++) {
    if (uid.equalsIgnoreCase(students[i].uid)) return i;
  }
  return -1;
}

// ---------- Send to Google Sheets ----------
bool sendAttendance(const String& name) {
  if (!connectWiFi()) return false;

  WiFiClientSecure client;
  client.setInsecure();  // simple HTTPS without certificate checking

  HTTPClient http;
  String url = String(SCRIPT_URL) + "?name=" + urlEncode(name);
  http.begin(client, url);
  http.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);  // Google redirects the response
  http.setTimeout(15000);

  int code = http.GET();
  Serial.print("HTTP response code: ");
  Serial.println(code);
  if (code > 0) {
    Serial.print("Server says: ");
    Serial.println(http.getString());
  }
  http.end();

  return code == 200;
}

// ---------- Setup ----------
void setup() {
  Serial.begin(115200);
  delay(500);

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);
  rfid.PCD_Init();
  delay(50);

  Serial.println();
  Serial.println("=== RFID Attendance System ===");

  byte version = rfid.PCD_ReadRegister(MFRC522::VersionReg);
  if (version == 0x00 || version == 0xFF) {
    Serial.println("WARNING: No communication with RC522! Check wiring/power (3.3V).");
  } else {
    Serial.println("RC522 detected OK.");
  }

  connectWiFi();
  beep(100);
  Serial.println("Ready. Tap a card...");
}

// ---------- Loop ----------
void loop() {
  if (!rfid.PICC_IsNewCardPresent()) return;
  if (!rfid.PICC_ReadCardSerial()) return;

  String uid = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) uid += "0";
    uid += String(rfid.uid.uidByte[i], HEX);
  }
  uid.toUpperCase();

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();

  // Ignore the same card scanned again within the cooldown time
  if (uid == lastUid && millis() - lastScanTime < COOLDOWN_MS) {
    return;
  }
  lastUid = uid;
  lastScanTime = millis();

  Serial.println("--------------------------");
  Serial.print("Card ID: ");
  Serial.println(uid);

  int idx = findStudent(uid);
  if (idx < 0) {
    Serial.println("Unknown card - not registered.");
    beepUnknown();
    return;
  }

  String name = students[idx].name;
  Serial.print("Student: ");
  Serial.println(name);
  beepValid();

  if (sendAttendance(name)) {
    Serial.println("Attendance saved to Google Sheet.");
  } else {
    Serial.println("Failed to save attendance!");
    beepError();
  }
}
