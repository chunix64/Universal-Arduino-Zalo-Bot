// ===============================================
// FreeRTOS ZALO BOT Template
// ===============================================
// Features:
// - Persistent multi-user subscription list
// - Real-time chatbot handling via FreeRTOS tasks
#include <Preferences.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalZaloBot.h>

// ==================== CONFIGURATION ====================
constexpr const char* SSID = "YOUR_WIFI_SSID";
constexpr const char* PASSWORD = "YOUR_WIFI_PASSWORD";
constexpr const char* BOT_TOKEN = "YOUR_ZALO_BOT_TOKEN";

// ==================== USER LIMIT ====================
constexpr size_t MAX_USERS = 3;

// ==================== FREERTOS ====================
TaskHandle_t mainTaskHandle = NULL;
TaskHandle_t zaloReceiverTaskHandle = NULL;
TaskHandle_t zaloSenderTaskHandle = NULL;
TaskHandle_t wifiReconnectTaskHandle = NULL;

String users[MAX_USERS] = {};
WiFiClientSecure receiverClient;
WiFiClientSecure senderClient;
Preferences prefs;

UniversalZaloBot zaloReceiver(BOT_TOKEN, receiverClient, true);
UniversalZaloBot zaloSender(BOT_TOKEN, senderClient, true);

void setup() {
  Serial.begin(115200);
  setupHardware();
  setupWiFi();
  setupFreeRTOS();
  setupPreference();
  setupZaloBot();
  createTasks();

  Serial.println("System ready! IP: " + WiFi.localIP().toString());
}

void loop() {
}

//---------------------------------------------------------
//
// Setup functions
//
//---------------------------------------------------------

void setupHardware() {
  // Initialize hardware (GPIO, peripherals, etc.)
}

void setupFreeRTOS() {
  // Initialize FreeRTOS primitives (mutex, queues, etc.)
}

void setupWiFi() {
  WiFi.setAutoReconnect(true);
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
}

void setupPreference() {
  loadUsers();
}

void setupZaloBot() {
  receiverClient.setInsecure();
  senderClient.setInsecure();

  zaloReceiver.onCommand("/subscribe", handleSubscribe);
  zaloReceiver.onCommand("/unsubscribe", handleUnsubscribe);
  zaloReceiver.onCommand("/list", handleList);
  zaloReceiver.onCommand("/get_system_info", handleGetSystemInfo);
  zaloReceiver.onCommand("/tell", handleTell);

  zaloReceiver.begin();
  zaloSender.begin();
}

void createTasks() {
  xTaskCreatePinnedToCore(
    zaloReceiverTask,
    "zaloReceiverTask",
    8192,
    NULL,
    4,
    &zaloReceiverTaskHandle,
    0);

  xTaskCreatePinnedToCore(
    zaloSenderTask,
    "zaloSenderTask",
    8192,
    NULL,
    3,
    &zaloSenderTaskHandle,
    0);

  xTaskCreatePinnedToCore(
    mainTask,
    "mainTask",
    4096,
    NULL,
    2,
    &mainTaskHandle,
    1);

  xTaskCreatePinnedToCore(
    wifiReconnectTask,
    "wifiReconnectTask",
    2048,
    NULL,
    1,
    &wifiReconnectTaskHandle,
    0);
}


//---------------------------------------------------------
//
// Tasks
//
//---------------------------------------------------------

void mainTask(void* pvParameters) {
  while (true) {
    // Main application task
    // Place your core logic here

    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void zaloReceiverTask(void* pvParameters) {
  // Zalo receiver task
  // Handles incoming updates and dispatches commands
  // Do not modify unless you know what you're doing
  while (true) {
    zaloReceiver.handleUpdate();
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void zaloSenderTask(void* pvParameters) {
  while (true) {
    // You shouldn't use this task for normal bot logic.
    // Use built-in handlers instead: onText, onCommand, etc.

    // This task is designed for hardware events.
    // Example: isAlert == true -> send message.

    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void wifiReconnectTask(void* pvParameters) {
  // WiFi watchdog task
  // Reconnects automatically if connection is lost
  while (true) {
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("WiFi lost → reconnecting...");
      WiFi.reconnect();
    }
    vTaskDelay(pdMS_TO_TICKS(30000));
  }
}

//---------------------------------------------------------
//
// Zalo event handlers
//
//---------------------------------------------------------

void handleSubscribe(const Message& message) {
  if (addUser(message.userId)) {
    zaloSender.sendMessage(message.userId, "Subscribed successfully!");
  } else {
    zaloSender.sendMessage(message.userId, "You have already been in the list!");
  }
}

void handleUnsubscribe(const Message& message) {
  if (removeUser(message.userId)) {
    zaloSender.sendMessage(message.userId, "Unsubscribed successfully!");
  } else {
    zaloSender.sendMessage(message.userId, "You are not in the list!");
  }
}

void handleList(const Message& message) {
  zaloSender.sendMessage(message.userId, listUser());
}

void handleGetSystemInfo(const Message& message) {
  String out;

  out += "=== SYSTEM INFO ===\n";

  // --- WiFi ---
  out += "[WiFi]\n";
  out += "SSID: " + String(WiFi.SSID()) + "\n";
  out += "IP: " + WiFi.localIP().toString() + "\n";
  out += "RSSI: " + String(WiFi.RSSI()) + " dBm\n";
  out += "Status: " + String(WiFi.status() == WL_CONNECTED ? "Connected" : "Disconnected") + "\n\n";

  // --- Timing ---
  out += "[Timing]\n";
  out += "Uptime: " + String(millis() / 1000) + " sec\n";

  // --- Memory ---
  out += "[Memory]\n";
  out += "Free heap: " + String(ESP.getFreeHeap()) + " bytes\n";
  out += "Min free heap: " + String(ESP.getMinFreeHeap()) + " bytes\n\n";

  // --- Chip info ---
  out += "[Chip]\n";
  out += "Cores: " + String(ESP.getChipCores()) + "\n";
  out += "CPU freq: " + String(ESP.getCpuFreqMHz()) + " MHz\n";
  out += "Flash size: " + String(ESP.getFlashChipSize() / (1024 * 1024)) + " MB\n\n";

  // --- Users ---
  out += "[Users]\n";
  out += listUser() + "\n";

  // --- Tasks ---
  out += "\n[Tasks]\n";
  out += "MainTask: " + String(mainTaskHandle ? "Running" : "NULL") + "\n";
  out += "ReceiverTask: " + String(zaloReceiverTaskHandle ? "Running" : "NULL") + "\n";
  out += "SenderTask: " + String(zaloSenderTaskHandle ? "Running" : "NULL") + "\n";

  zaloSender.sendMessage(message.userId, out);
}

void handleTell(const Message& message) {
  String text = extractArgument(message.content, "/tell");
  sendMessageToAll(text);
}

//---------------------------------------------------------
//
// Helpers
//
//---------------------------------------------------------

void loadUsers() {
  prefs.begin("zalo", true);

  for (int i = 0; i < MAX_USERS; i++) {
    users[i] = prefs.getString(("user" + String(i)).c_str(), "");
  }

  prefs.end();
}

void saveUsers() {
  prefs.begin("zalo", false);

  for (int i = 0; i < MAX_USERS; i++) {
    prefs.putString(("user" + String(i)).c_str(), users[i]);
  }

  prefs.end();
}

bool addUser(String newUser) {

  for (int i = 0; i < MAX_USERS; i++) {
    if (users[i] == newUser) {
      return false;
    }
  }

  String removedUser = users[MAX_USERS - 1];

  for (int i = MAX_USERS - 1; i > 0; i--) {
    users[i] = users[i - 1];
  }

  users[0] = newUser;

  saveUsers();

  if (removedUser.length() > 0) {
    zaloSender.sendMessage(removedUser, "A new user registered, so you were removed due to the maximum user limit.");
  }

  return true;
}

bool removeUser(String user) {
  bool isfound = false;

  for (int i = 0; i < MAX_USERS; i++) {
    if (users[i] == user) {
      isfound = true;
    }

    if (isfound && i < MAX_USERS - 1) {
      users[i] = users[i + 1];
    }
  }

  if (isfound) {
    users[MAX_USERS - 1] = "";
    saveUsers();
  }

  return isfound;
}

String listUser() {
  String out = "Registered users (max: " + String(MAX_USERS) + "):\n";

  bool first = true;

  for (int i = 0; i < MAX_USERS; i++) {
    if (users[i].length() > 0) {

      if (!first) {
        out += "\n";
      }

      out += String(i + 1) + ". " + users[i];
      first = false;
    }
  }

  if (first) {
    return "No registered users.";
  }

  return out;
}

String extractArgument(const String& command, const String& prefix) {
  String arg = command;
  if (arg.startsWith(prefix)) {
    arg = arg.substring(prefix.length());
  }
  arg.trim();
  return arg;
}

void sendMessageToAll(const String& message) {
  for (int i = 0; i < MAX_USERS; i++) {
    if (users[i].length() > 0) {
      zaloSender.sendMessage(users[i], message);
    }
  }
}