#define BLYNK_TEMPLATE_ID "TMPL6j9AkKu4c"
#define BLYNK_TEMPLATE_NAME "board arduino"
#define BLYNK_AUTH_TOKEN "WcXILyKmeZKrExVzVdFOhCBYwd-n-ZWT"
#define BLYNK_PRINT Serial  // Debug output

#include <Wire.h>
#include <RTClib.h>
#include <SoftwareSerial.h>
#include <BlynkSimpleShieldEsp8266.h>

#define BUZZER_PIN 7

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "tes";
char pass[] = "HURUFKECIK";

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
    alreadyBuzzed = false;
    Serial.print("Alarm set: ");
    Serial.print(targetHour);
    Serial.print(":");
    Serial.println(targetMinute);
  }
}

BLYNK_WRITE(V1) {
  customMessage = param.asString();
  Serial.print("Notification: ");
  Serial.println(customMessage);
}

void checkTime() {
  DateTime now = rtc.now();

  if (now.hour() == targetHour && now.minute() == targetMinute && !alreadyBuzzed) {
    digitalWrite(BUZZER_PIN, HIGH);
    Blynk.logEvent("minum_obat", customMessage);
    delay(1000);
    digitalWrite(BUZZER_PIN, LOW);
    alreadyBuzzed = true;
  }
  
  // Reset flag jika sudah lewat waktu alarm
  if (now.hour() != targetHour || now.minute() != targetMinute) {
    alreadyBuzzed = false;
  }
}

void setup() {
  Serial.begin(9600);
  delay(1000); // Tunggu serial ready
  
  Serial.println("Starting system...");

  // Inisialisasi RTC
  Wire.begin();
  if (!rtc.begin()) {
    Serial.println("Could not find RTC!");
    while (1);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, setting to compile time");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  // Konfigurasi ESP8266
  EspSerial.begin(115200); // ESP8266-01 biasanya menggunakan 115200 baud
  delay(1000);

  Serial.println("Connecting to WiFi...");
  Blynk.begin(auth, wifi, ssid, pass, "blynk.cloud", 80);
  
  timer.setInterval(1000L, checkTime);
  Serial.println("System ready!");
}

void loop() {
  Blynk.run();
  timer.run();
  
  // Debug connection status
  static unsigned long lastReport = 0;
  if (millis() - lastReport > 5000) {
    lastReport = millis();
    Serial.print("WiFi status: ");
    Serial.println(WiFi.status());
    Serial.print("Blynk connection: ");
    Serial.println(Blynk.connected() ? "Connected" : "Disconnected");
  }
}
