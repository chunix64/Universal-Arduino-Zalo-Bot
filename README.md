# Universal Zalo Bot Library

An Arduino library for interacting with the **Zalo Bot API**, designed to work across multiple platforms including ESP32, ESP8266, and Arduino boards with WiFi modules.

> ⚠️ This library is under active development. APIs and features may change.

## 📌 Introduction

This library provides a simple interface for the [Zalo Bot API](https://bot.zaloplatforms.com/docs/call-api/), allowing Arduino-based devices to communicate with Zalo bots.

Zalo is an instant messaging platform that supports bots for automation and remote interaction. With this library, you can:

- Send status updates from IoT devices
- Control hardware remotely via Zalo messages
- Integrate Arduino projects with mobile notifications

## 🚀 Features

- [x] Send messages via Zalo Bot API  
- [ ] Receive messages (planned)  
- [ ] Event handling system (planned)  
- [ ] Long polling support (planned)  
- [ ] Webhook support (planned)  
- [ ] Async / non-blocking operations (planned)  

## 📦 Supported Boards

- ESP32  
- ESP8266  
- Arduino Uno / Mega + WiFi shield / ESP01
- Most Arduino-compatible boards with networking support

## 🛠 Installation

### Option 1: Install via Arduino IDE (ZIP)

1. Download the library:  
   👉 https://github.com/chunix64/Universal-Arduino-Zalo-Bot/archive/refs/heads/main.zip

2. Open Arduino IDE

3. Go to:

```
Sketch → Include Library → Add .ZIP Library...

````

4. Select the downloaded ZIP file

### Option 2: Manual Installation (Git Clone)

Clone the repository into your Arduino libraries folder:

```bash
git clone https://github.com/chunix64/Universal-Arduino-Zalo-Bot.git
````

Then restart Arduino IDE.

## 📚 Dependencies

This library requires:

* [ArduinoJson](https://github.com/bblanchon/ArduinoJson) by Benoît Blanchon

Install it via Library Manager or manually from GitHub.

## 🌱 Getting Started

### Create a zalo bot
To generate your new Bot, you need an Access Token. Talk to [Zalo Bot Manager](http://zalo.me/3899658094114941620) and follow a few simple steps described [here](https://bot.zaloplatforms.com/docs/create-bot/).

### Getting Your Chat ID

1. Open the following URL in your browser:

```
https://bot-api.zaloplatforms.com/bot<YOUR_BOT_TOKEN>/getUpdates
```

- With `YOUR_BOT_TOKEN` is your bot token from the previous step;

2. Send a message to your bot on Zalo

3. Look for this field in the JSON response:

```
message.chat.id
```

That value is your `CHAT_ID`.

> This current method is temporary. A simpler method is planned.

### Use the library
Include UniversalZaloBot in your project:

```ino
#include <UniversalZaloBot.h>
```

and pass it your bot token and your chat id, see the example for more details.

## 💡 Basic Usage Example

### Send a message on boot

```cpp
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalZaloBot.h>

const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

#define BOT_TOKEN "YOUR_ZALO_BOT_TOKEN"
#define CHAT_ID  "YOUR_ZALO_CHAT_ID"

WiFiClientSecure client;
UniversalZaloBot zalo(BOT_TOKEN, client);

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  client.setInsecure(); // skip certificate validation

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  zalo.sendMessage(CHAT_ID, "Salve!");
}

void loop() {
}
```

## 🤖 Library API
Here is a list of the main features that this library covers.

| Features                | Description                                          | Usage                                                                                        |
|-------------------------|------------------------------------------------------|----------------------------------------------------------------------------------------------|
| _Sending messages_      | Your bot can send messages to any Zalo account       | `bool sendMessage(const String &chat_id, const String &message)`                             |
| _Sending photos_        | Your bot can send photos to any Zalo account         | `bool sendPhoto(const String &chat_id, const String &photo_url, const String &caption = "")` |
| _Sending stickers_      | Your bot can send stickers to any Zalo account       | `bool sendSticker(const String &chat_id, const String &sticker_id)`                          |
| _Sending chat action_   | Your bot can send chat actions to any Zalo account   | `bool sendChatAction(const String &chat_id, const String &action);`                          |


## 🧭 Roadmap

* [ ] Incoming message support
* [ ] Webhook integration
* [ ] Event-driven architecture
* [ ] Async request handling
* [ ] Multi-thread support (FreeRTOS)

## ⚠️ Notes

* This library is experimental and may change frequently.
* Not all Zalo Bot API features are implemented yet.
* Contributions and feedback are welcome.
* Zalo currently supports a limit of 3,000 sent messages per month. Please use it carefully.

## 📄 License

GNU GPL v3

## 🤝 Credits

* Inspired by [Universal Arduino Telegram Bot](https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot)
* Built for Arduino & ESP-based IoT projects
