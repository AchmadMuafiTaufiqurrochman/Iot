#include <WiFi.h>
#include <WiFiClient.h>
#include <HTTPClient.h>

#define MQ2_PIN 34
#define BUZZER_PIN 5

#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""
#define WIFI_CHANNEL 6

const char* serverName = "https://sgqztz3w-5000.asse.devtunnels.ms/";

void setup() {
  Serial.begin(115200);

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD, WIFI_CHANNEL);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(200);
  }
  Serial.println(" Connected!");
}

void loop() {

  int raw = analogRead(MQ2_PIN);
  long timestamp = millis();

  String status = (raw >= 3762) ? "DARURAT" : "AMAN";

  Serial.print("ADC: ");
  Serial.print(raw);
  Serial.print(" | Status: ");
  Serial.println(status);

  // BUZZER SIMPLE
  if (raw >= 3762) {
    digitalWrite(BUZZER_PIN, LOW);   // bunyi
  } else {
    digitalWrite(BUZZER_PIN, HIGH);    // mati
  }

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverName);
    http.addHeader("Content-Type", "application/json");

    String jsonData =
      "{"
      "\"adc\": \"" + String(raw) +
      "\", \"status\": \"" + status +
      "\", \"timestamp\": \"" + String(timestamp) +
      "\"}";

    int resp = http.POST(jsonData);
    Serial.print("API Resp: ");
    Serial.println(resp);

    http.end();
  }

  delay(2000);
}
