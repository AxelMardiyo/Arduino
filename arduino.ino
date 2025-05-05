#define BLYNK_TEMPLATE_ID "TMPL6_Fwa4GoS"
#define BLYNK_TEMPLATE_NAME "medicine alarm"
#define BLYNK_AUTH_TOKEN "mTlU6xMVki1zz459t5aDDo2zyU6ZH33b"

#include <Wire.h>
#include <RTClib.h>
#include <SoftwareSerial.h>
#include <WiFi.h>             // Ganti dengan WiFi.h untuk ESP32 atau gunakan ESP8266WiFi.h untuk ESP8266
#include <BlynkSimpleEsp32.h> // Ganti dengan BlynkSimpleEsp8266.h untuk ESP8266

#define BUZZER_PIN 7

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Abcd";    // Ganti dengan SSID WiFi kamu
char pass[] = "abcd1234"; // Ganti dengan password WiFi kamu

// SoftwareSerial untuk komunikasi ke ESP-01
SoftwareSerial espSerial(2, 3); // RX, TX
WiFiClient client;  // Menggunakan WiFiClient untuk ESP32 atau ESP8266

RTC_DS3231 rtc;
BlynkTimer timer;

int targetHour = -1;
int targetMinute = -1;
String customMessage = "Waktunya minum obat!";

// Ambil waktu dari widget Time Input di V0
BLYNK_WRITE(V0) {
  TimeInputParam t(param);
  if (t.hasStartTime()) {
    targetHour = t.getStartHour();
    targetMinute = t.getStartMinute();
    Serial.print("Jadwal diset: ");
    Serial.print(targetHour);
    Serial.print(":");
    Serial.println(targetMinute);
  }
}

// Ambil pesan kustom dari V1
BLYNK_WRITE(V1) {
  customMessage = param.asString();
  Serial.print("Pesan notifikasi diset: ");
  Serial.println(customMessage);
}

void checkTime() {
  DateTime now = rtc.now();

  if (now.hour() == targetHour && now.minute() == targetMinute && now.second() == 0) {
    digitalWrite(BUZZER_PIN, HIGH);
    Blynk.logEvent("minum_obat", customMessage); // Pastikan event dibuat di Blynk dashboard
    delay(1000);
    digitalWrite(BUZZER_PIN, LOW);
  }
}

void setup() {
  Serial.begin(9600);
  espSerial.begin(9600);  // komunikasi ke ESP-01
  
  Wire.begin();
  rtc.begin();

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  // Menghubungkan ke Blynk IoT
  Blynk.begin(auth, ssid, pass);

  // Mengatur timer untuk memeriksa waktu setiap detik
  timer.setInterval(1000L, checkTime);

  Serial.println("Sistem siap.");
}

void loop() {
  Blynk.run();  // ← pastikan pakai tanda kurung
  timer.run();  // ← pastikan pakai tanda kurung juga
}
