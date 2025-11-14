#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

#define MQ2_PIN 34
#define BUZZER_PIN 5

#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""
#define WIFI_CHANNEL 6

// Gunakan path lengkap API Laravel (routes/api.php) — prefix `/api`
const char* serverName = "https://sgqztz3w-8000.asse.devtunnels.ms/api/sensor-data";

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
    // Untuk dev-tunnel dengan sertifikat tidak dikenali (testing)
    WiFiClientSecure client;
    client.setInsecure();
    http.begin(client, serverName);
    http.addHeader("Content-Type", "application/json");

    // Build JSON safely, kirim adc dan timestamp sebagai numeric
    String jsonData = String("{\"adc\": ") + String(raw) +
                      String(", \"status\": \"") + status + String("\"") +
                      String(", \"timestamp\": ") + String(timestamp) + String("}");

    int resp = http.POST(jsonData);
    String respBody = http.getString();

    Serial.print("API Resp code: ");
    Serial.println(resp);
    Serial.print("API Resp body: ");
    Serial.println(respBody);

    http.end();
  }

  delay(2000);
}
