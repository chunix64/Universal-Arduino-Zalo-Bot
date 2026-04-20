/*
  ============================================================
  FreeRTOS ESP32 Template (UniversalZaloBot)
  ============================================================

  FEATURES:
  - WiFi initialization
  - Dedicated FreeRTOS tasks (multi-core ESP32)
  - Zalo bot runs in a separate task (recommended: Core 0)
  - Main application logic runs in another task (Core 1)

  IMPORTANT RULES:
  ------------------------------------------------------------

  1. DO NOT use delay()
     → Always use:
        vTaskDelay(pdMS_TO_TICKS(ms))

     Example:
        delay(13)
        → vTaskDelay(pdMS_TO_TICKS(13))

  ------------------------------------------------------------

  2. Keep network tasks on Core 0 to avoid crashes (RECOMMENDED)

     Network-related tasks include:
     - Zalo bot handling
     - HTTP / HTTPS requests
     - MQTT / WebSocket clients

  ------------------------------------------------------------

  3. Keep application logic on Core 1
     - sensors
     - control logic
     - UI / state machines

  ------------------------------------------------------------

  4. ESP32 architecture:
     Core 0 → system tasks + WiFi stack
     Core 1 → user application tasks
*/

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalZaloBot.h>

const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

#define BOT_TOKEN "YOUR_ZALO_BOT_TOKEN"

WiFiClientSecure client;
UniversalZaloBot zalo(BOT_TOKEN, client, true);

TaskHandle_t zaloTaskHandle = NULL;
TaskHandle_t mainTaskHandle = NULL;

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    vTaskDelay(pdMS_TO_TICKS(500));
  }

  Serial.println("\nWiFi connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  client.setInsecure();  // skip certificate validation

  zalo.begin();

  /* -------------------------
     Create Zalo Task (Core 0)
     Network-sensitive task
     ------------------------- */
  xTaskCreatePinnedToCore(
    zaloTask,
    "ZaloTask",
    8192,  // stack size (increase if crashes occur)
    NULL,
    1,  // priority (medium-low)
    &zaloTaskHandle,
    0  // Core 0
  );

  /* -------------------------
     Create Main Task (Core 1)
     Application logic
     ------------------------- */
  xTaskCreatePinnedToCore(
    mainTask,
    "MainTask",
    4096,  // stack size (increase if crashes occur)
    NULL,
    2,  // priority (higher than ZaloTask if needed)
    &mainTaskHandle,
    1  // Core 1
  );
}

/* ============================================================
   ZALO TASK (CORE 0)
============================================================ */
void zaloTask(void* pvParameters) {
  for (;;) {
    zalo.handleUpdate();

    // Example placeholder logic:
    // if (something) {
    //  zalo.sendMessage(YOUR_CHAT_ID, "Hey! Something happened.");
    // }

    // Yield CPU to prevent watchdog/reset issues
    vTaskDelay(pdMS_TO_TICKS(1));
  }
}

/* ============================================================
   MAIN APPLICATION TASK (CORE 1)
============================================================ */
void mainTask(void* pvParameters) {
  for (;;) {

    // Example placeholder logic:
    // Serial.println("Main task running");

    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

/* ============================================================
   LOOP (NOT USED)
   ------------------------------------------------------------
   ESP32 FreeRTOS replaces Arduino loop()
   Keep this empty.
============================================================ */
void loop() {
  // intentionally empty
}
