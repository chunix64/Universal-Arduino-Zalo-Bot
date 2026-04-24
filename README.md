# Universal Zalo Bot Library
[![en](https://img.shields.io/badge/lang-en-blue.svg)](https://github.com/chunix64/Universal-Arduino-Zalo-Bot/blob/main/README.md)
[![vn](https://img.shields.io/badge/lang-vn-yellow.svg)](https://github.com/chunix64/Universal-Arduino-Zalo-Bot/blob/main/README.vn.md)

An Arduino library for interacting with the Zalo Bot API, designed to work across multiple platforms including ESP32, ESP8266, and Arduino boards with WiFi modules.

## Installation
### Option 1: Install via Arduino IDE (ZIP)

1. Download the library: [https://github.com/chunix64/Universal-Arduino-Zalo-Bot/archive/refs/heads/main.zip](https://github.com/chunix64/Universal-Arduino-Zalo-Bot/archive/refs/heads/main.zip)

2. Open Arduino IDE

3. Go to:

```
Sketch → Include Library → Add .ZIP Library...
````

4. Select the downloaded ZIP file

### Option 2: Manual Installation

Clone the repository or extract the .Zip library into your Arduino libraries folder:

```bash
git clone https://github.com/chunix64/Universal-Arduino-Zalo-Bot.git
````

Then restart Arduino IDE

## Dependencies

This library requires:

* [ArduinoJson](https://github.com/bblanchon/ArduinoJson)

Install it via Library Manager or manually from GitHub.

## Usage
### Create a Zalo bot
To generate your new Bot, you need an Access Token. Talk to [Zalo Bot Manager](http://zalo.me/3899658094114941620) and follow a few simple steps described [here](https://bot.zaloplatforms.com/docs/create-bot/).

### Setup your bot
```ino
UniversalZaloBot zalo(BOT_TOKEN, client);
```

For FreeRTOS mode:
```ino
UniversalZaloBot zalo(BOT_TOKEN, client, true);
```

where `BOT_TOKEN` is your Zalo bot access token and `client` is your network client.

### Basic setup
```ino
zalo.begin(); // optional but needed by FreeRTOS mode
```

## Example
### Print received messages to console
```ino
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

  zalo.begin();  // optional

  Serial.println("");
  Serial.print("Connected to Wifi. Local IP: ");
  Serial.println(WiFi.localIP());

  zalo.onText([](const Message& message) {
    Serial.println("---- New Message ----");
    Serial.print("Chat ID: ");
    Serial.println(message.chatId);

    Serial.print("User ID: ");
    Serial.println(message.userId);

    Serial.print("User Name: ");
    Serial.println(message.userName);

    Serial.print("Content: ");
    Serial.println(message.content);
    Serial.println("---------------------");
  });
}

void loop() {
  zalo.handleUpdate();
  delay(1);
}
```

### More examples
You can find more examples [here](https://github.com/chunix64/Universal-Arduino-Zalo-Bot/tree/main/examples)

## Library API
Here is a list of the main features that this library covers.

| Features                | Description                                          | Usage                                                                                        |
|-------------------------|------------------------------------------------------|----------------------------------------------------------------------------------------------|
| _Begin_                 | Optional                                             | `void begin()`                                                                               |
| _Debug_                 | Show logs and useful data                            | `setDebug(bool isDebug)`                                                                     |
| _Sending messages_      | Your bot can send messages to any Zalo account       | `bool sendMessage(const String &chat_id, const String &message)`                             |
| _Sending photos_        | Your bot can send photos to any Zalo account         | `bool sendPhoto(const String &chat_id, const String &photo_url, const String &caption = "")` |
| _Sending stickers_      | Your bot can send stickers to any Zalo account       | `bool sendSticker(const String &chat_id, const String &sticker_id)`                          |
| _Sending chat action_   | Your bot can send chat actions to any Zalo account   | `bool sendChatAction(const String &chat_id, const String &action)`                           |
| _Receive messages_      | Your bot can receive messages via long polling       | `Message getUpdates()`                                                                       |
| _Handle updates_ | Your bot can handle incoming message updates in real time (long polling).<br><br>Required for `onText`, etc.<br><br>Should be used inside an infinite loop | `void handleUpdate()` |
| _onText_                | Handles callbacks triggered by incoming text messages    | `void onText(ZaloEventCallback callback)`                                                |
| _onPhoto_               | Handles callbacks triggered by incoming photo messages   | `void onPhoto(ZaloEventCallback callback)`                                               |
| _onSticker_             | Handles callbacks triggered by incoming sticker messages | `void onSticker(ZaloEventCallback callback)`                                             |
| _onUpdate_              | Handles callbacks triggered by any incoming messages     | `void onUpdate(ZaloEventCallback callback)`                                              |
| _onCommand_             | Handles callbacks triggered by any matched messenges (startswith) <br><br> **> ESP32 and ESP8266 only** | `void onCommand(const String &command, ZaloEventCallback callback)` |

## Notes
* Zalo currently supports a limit of 3,000 sent messages per month. Please use it carefully.
* Zalo does not support long polling offsets, so messages may be lost if they are sent too quickly.
* Parallel, non-blocking mode is supported with FreeRTOS (ESP32).
* When using FreeRTOS mode, the network task (Zalo) must run on Core 0 to avoid crashes.
* Use FreeRTOS to manage tasks efficiently by using two separate instances—one for receiving and one for sending—so the receiver doesn’t block the sender with long polling.

## Roadmap

* [ ] Webhook integration
* [ ] Add more FreeRTOS supported boards to the library
* [ ] Add more examples
* [ ] Reduce memory and ROM costs
* [ ] Support PlatformIO

## Contributing

Pull requests are welcome. For major changes, please open an issue first
to discuss what you would like to change.

Please make sure to update tests as appropriate.

## License

This project is licensed under the [Apache License 2.0](https://www.apache.org/licenses/LICENSE-2.0).

## Credits
* Maintainer: [chunix64](https://github.com/chunix64)
* Inspired by [Universal Arduino Telegram Bot](https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot)
* Built for Arduino & ESP-based IoT projects
