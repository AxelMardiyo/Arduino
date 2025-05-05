#define BLYNK_TEMPLATE_ID "TMPL6_Fwa4GoS"
#define BLYNK_TEMPLATE_NAME "medicine alarm"
#define BLYNK_AUTH_TOKEN  "mTlU6xMVki1zz459t5aDDo2zyU6ZH33b"

#include <Wire.h>
#include <RTClib.h>
#include <SoftwareSerial.h>
#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>

#define BUZZER_PIN 7

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Abcd";
char pass[] = "abcd1234";

// SoftwareSerial untuk komunikasi ke ESP-01
SoftwareSerial espSerial(2, 3); // RX, TX
ESP8266 wifi(&espSerial);       // objek wifi dibikin dari espSerial

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
  espSerial.begin(9600); // komunikasi ke ESP-01

  Wire.begin();
  rtc.begin();

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  Blynk.begin(auth, wifi, ssid, pass);

  timer.setInterval(1000L, checkTime);

  Serial.println("Sistem siap.");
}

void loop() {
  Blynk.run();   // ← harus pakai tanda kurung ()
  timer.run();   // ← harus pakai tanda kurung juga
}
