#define BLYNK_TEMPLATE_ID "TMPL6_Fwa4GoS"
#define BLYNK_TEMPLATE_NAME "medicine alarm"
#define BLYNK_AUTH_TOKEN "mTlU6xMVki1zz459t5aDDo2zyU6ZH33b"

#include <Wire.h>
#include <RTClib.h>
#include <SoftwareSerial.h>
#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>

#define BUZZER_PIN 7

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Abcd";
char pass[] = "abcd1234";

// RX ke TX ESP, TX ke RX ESP
SoftwareSerial EspSerial(2, 3); // RX, TX
ESP8266 wifi(EspSerial);

RTC_DS3231 rtc;
BlynkTimer timer;

// Variabel alarm
int targetHour = -1;
int targetMinute = -1;
String customMessage = "Waktunya minum obat!";
bool alreadyBuzzed = false;

BLYNK_WRITE(V0) {
  TimeInputParam t(param);
  if (t.hasStartTime()) {
    targetHour = t.getStartHour();
    targetMinute = t.getStartMinute();
    alreadyBuzzed = false; // Reset status alarm
    Serial.print("Alarm diset ke: ");
    Serial.print(targetHour);
    Serial.print(":");
    Serial.println(targetMinute);
  }
}

BLYNK_WRITE(V1) {
  customMessage = param.asString();
  Serial.print("Pesan notifikasi: ");
  Serial.println(customMessage);
}

void checkTime() {
  DateTime now = rtc.now();

  if (now.hour() == targetHour && now.minute() == targetMinute) {
    if (!alreadyBuzzed) {
      digitalWrite(BUZZER_PIN, HIGH);
      Blynk.logEvent("minum_obat", customMessage);
      delay(1000);
      digitalWrite(BUZZER_PIN, LOW);
      alreadyBuzzed = true;
    }
  } else {
    alreadyBuzzed = false; // Reset alarm jika sudah lewat
  }
}

void setup() {
  Serial.begin(9600);
  EspSerial.begin(9600);
  
  Wire.begin();
  if (!rtc.begin()) {
    Serial.println("RTC tidak ditemukan!");
    while (1);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC disetel ke waktu kompilasi.");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  Blynk.begin(auth, wifi, ssid, pass);
  timer.setInterval(1000L, checkTime);
  
  Serial.println("Sistem siap!");
}

void loop() {
  Blynk.run();
  timer.run();
}
