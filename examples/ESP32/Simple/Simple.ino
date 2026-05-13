#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalZaloBot.h>

const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

#define BOT_TOKEN "YOUR_ZALO_BOT_TOKEN"

WiFiClientSecure client;
UniversalZaloBot zalo(BOT_TOKEN, client);

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  client.setInsecure();  // skip certificate validation

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.print("Connected to Wifi. Local IP: ");
  Serial.println(WiFi.localIP());

  zalo.onText([](const Message& message) {
    String log =
      "---- New Message ----\n"
      "Chat ID: " + String(message.chatId) + "\n" +
      "User ID: " + String(message.userId) + "\n" +
      "User Name: " + String(message.userName) + "\n" +
      "Content: " + String(message.content) + "\n" +
      "---------------------";

    Serial.println(log);
    zalo.sendMessage(message.userId, log);
  });
}

void loop() {
  zalo.handleUpdate();
  delay(1);
}
