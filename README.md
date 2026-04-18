# arduino-zalo-bot

Zalo Bot API client for Arduino.

> ⚠️ This library is still under active development. Expect breaking changes.

---

## Features

* Send messages via Zalo Bot API
* Lightweight and designed for embedded use (ESP32 / ESP8266 / Arduino with wifi module / etc)
* Simple API interface

---

## Installation

1. Clone this repository into your Arduino `libraries` directory or download zip and extract:

   ```bash
   git clone https://github.com/your-repo/arduino-zalo-bot.git
   ```

2. Restart the Arduino IDE.

---

## Example

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

---

## Getting Your Chat ID

1. Open the following URL in your browser:

```
https://bot-api.zaloplatforms.com/bot<YOUR_BOT_TOKEN>/getUpdates
```

2. Send a message to your bot on Zalo

3. Look for this field in the JSON response:

```
message.chat.id
```

That value is your `CHAT_ID`.

> This current method is temporary. A simpler method is planned.
---

## Notes

* The library currently does **not** parse incoming updates automatically
* You must manually retrieve and inspect responses for now
* Automatic update handling will be added in a future release

---

## License

GNU GPL v3 (as stated in source code)
