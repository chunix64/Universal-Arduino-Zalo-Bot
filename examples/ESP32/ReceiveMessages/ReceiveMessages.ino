#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalZaloBot.h>

const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

#define BOT_TOKEN "YOUR_ZALO_BOT_TOKEN"

TaskHandle_t zaloTaskHandle = NULL;

WiFiClientSecure client;

// FreeRTOS multi-core suppport
UniversalZaloBot zalo(BOT_TOKEN, client, true);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  client.setInsecure();
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  
  zalo.begin();

  Serial.println("");
  Serial.println(WiFi.localIP());

  zalo.onText([](const Message &message) {
    Serial.print("Message received: ");
    Serial.println(message.content);
  });

  xTaskCreatePinnedToCore(
    zaloTask,
    "ZaloTask",
    8192,
    NULL,
    1,
    &zaloTaskHandle,
    0
  );
}

void zaloTask(void* pvParameters) {
  while(true) {
    zalo.handleUpdate();
    vTaskDelay(pdMS_TO_TICKS(1));
  }
}

void loop() {
  // put your main code here, to run repeatedly:
}
