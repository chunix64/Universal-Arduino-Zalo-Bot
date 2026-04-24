# Universal Zalo Bot
[![en](https://img.shields.io/badge/lang-en-blue.svg)](https://github.com/chunix64/Universal-Arduino-Zalo-Bot/blob/main/README.md)
[![vn](https://img.shields.io/badge/lang-vn-yellow.svg)](https://github.com/chunix64/Universal-Arduino-Zalo-Bot/blob/main/README.vn.md)

Một thư viện Arduino để tương tác với Zalo Bot API, được thiết kế để hoạt động trên nhiều nền tảng bao gồm ESP32, ESP8266, và các bo mạch Arduino có module WiFi.

## Cài Đặt
1. Mở **Arduino IDE**
2. Vào:

   ```
   Sketch → Include Library → Manage Libraries
   ```
3. Tìm kiếm:

   ```
   Universal Arduino Zalo Bot
   ```
4. Nhấn **Install** (Cài đặt)

## Phụ Thuộc

Thư viện này yêu cầu:

* [ArduinoJson](https://github.com/bblanchon/ArduinoJson)

Cài đặt qua Library Manager hoặc thủ công từ GitHub.

## Sử Dụng
### Tạo Zalo bot
Để tạo bot mới, bạn cần một Access Token. Nhắn tin tới [Zalo Bot Manager](http://zalo.me/3899658094114941620) và làm theo các bước đơn giản được mô tả [tại đây](https://bot.zaloplatforms.com/docs/create-bot/).

### Khởi tạo bot
Chế độ tiêu chuẩn:
```ino
UniversalZaloBot zalo(BOT_TOKEN, client);
```

Chế độ FreeRTOS:
```ino
UniversalZaloBot zalo(BOT_TOKEN, client, true);
```

Trong đó `BOT_TOKEN` là access token của Zalo bot và `client` là network client của bạn.

### Cấu hình cơ bản
```ino
zalo.begin(); // tùy chọn nhưng cần thiết cho chế độ FreeRTOS
```

## Ví Dụ
### In tin nhắn nhận được ra console
```ino
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalZaloBot.h>

const char* ssid = "TÊN_WIFI_CỦA_BẠN";
const char* password = "MẬT_KHẨU_WIFI_CỦA_BẠN";

#define BOT_TOKEN "TOKEN_ZALO_BOT_CỦA_BẠN"

WiFiClientSecure client;
UniversalZaloBot zalo(BOT_TOKEN, client);

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  client.setInsecure();  // bỏ qua xác thực chứng chỉ

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  zalo.begin();  // Có thể có hoặc không (optional)

  Serial.println("");
  Serial.print("Đã kết nối WiFi. IP nội bộ: ");
  Serial.println(WiFi.localIP());

  zalo.onText([](const Message& message) {
    Serial.println("---- Tin Nhắn Mới ----");
    Serial.print("Chat ID: ");
    Serial.println(message.chatId);

    Serial.print("User ID: ");
    Serial.println(message.userId);

    Serial.print("Tên người dùng: ");
    Serial.println(message.userName);

    Serial.print("Nội dung: ");
    Serial.println(message.content);
    Serial.println("----------------------");
  });
}

void loop() {
  zalo.handleUpdate();
  delay(1);
}
```

### Thêm ví dụ
Bạn có thể tìm thêm ví dụ [tại đây](https://github.com/chunix64/Universal-Arduino-Zalo-Bot/tree/main/examples)

## API Thư Viện
Dưới đây là danh sách các tính năng chính mà thư viện này cung cấp.

| Tính năng               | Mô tả                                                | Cách dùng                                                                                    |
|-------------------------|------------------------------------------------------|----------------------------------------------------------------------------------------------|
| _Begin_                 | Có thể có hoặc không (optional)                      | `void begin()`                                                                            |
| _Debug_                 | Hiển thị log và dữ liệu hữu ích                      | `setDebug(bool isDebug)`                                                                     |
| _Gửi tin nhắn_          | Bot có thể gửi tin nhắn tới bất kỳ tài khoản Zalo nào | `bool sendMessage(const String &chat_id, const String &message)`                           |
| _Gửi ảnh_               | Bot có thể gửi ảnh tới bất kỳ tài khoản Zalo nào    | `bool sendPhoto(const String &chat_id, const String &photo_url, const String &caption = "")` |
| _Gửi sticker_           | Bot có thể gửi sticker tới bất kỳ tài khoản Zalo nào | `bool sendSticker(const String &chat_id, const String &sticker_id)`                        |
| _Gửi chat action_       | Bot có thể gửi chat action tới bất kỳ tài khoản Zalo nào | `bool sendChatAction(const String &chat_id, const String &action)`                    |
| _Nhận tin nhắn_         | Bot có thể nhận tin nhắn qua long polling            | `Message getUpdates()`                                                                       |
| _Xử lý cập nhật_        | Bot có thể xử lý các cập nhật tin nhắn đến theo thời gian thực (long polling).<br><br>Bắt buộc cho `onText`, v.v.<br><br>Nên dùng trong vòng lặp vô hạn | `void handleUpdate()` |
| _onText_                | Xử lý callback khi nhận tin nhắn văn bản            | `void onText(ZaloEventCallback callback)`                                                    |
| _onPhoto_               | Xử lý callback khi nhận tin nhắn ảnh                | `void onPhoto(ZaloEventCallback callback)`                                                   |
| _onSticker_             | Xử lý callback khi nhận tin nhắn sticker            | `void onSticker(ZaloEventCallback callback)`                                                 |
| _onUpdate_              | Xử lý callback khi nhận bất kỳ tin nhắn nào         | `void onUpdate(ZaloEventCallback callback)`                                                  |
| _onCommand_             | Xử lý callback khi tin nhắn khớp lệnh (startswith) <br><br> **> Chỉ dành cho ESP32 và ESP8266** | `void onCommand(const String &command, ZaloEventCallback callback)` |

## Lưu Ý
* Zalo hiện giới hạn 3.000 tin nhắn gửi mỗi tháng. Vui lòng sử dụng cẩn thận.
* Zalo không hỗ trợ long polling offset, do đó tin nhắn có thể bị mất nếu gửi quá nhanh.
* Chế độ song song, không chặn luồng được hỗ trợ với FreeRTOS (ESP32).
* Khi sử dụng chế độ FreeRTOS, tác vụ mạng (Zalo) phải chạy trên Core 0 để tránh bị treo hoặc crash.
* Nên sử dụng FreeRTOS để quản lý tác vụ hiệu quả bằng cách sử dụng 2 instances—một để nhận và một để gửi—để quá trình nhận không chặn quá trình gửi do long polling

## Lộ Trình Phát Triển

* [ ] Tích hợp Webhook
* [ ] Thêm các bo mạch hỗ trợ FreeRTOS vào thư viện
* [ ] Thêm ví dụ
* [ ] Giảm chi phí bộ nhớ và ROM
* [ ] Hỗ trợ PlatformIO

## Đóng Góp

Pull request luôn được chào đón. Đối với các thay đổi lớn, vui lòng mở issue trước để thảo luận về những gì bạn muốn thay đổi.

Hãy đảm bảo cập nhật các bài kiểm thử khi cần thiết.

## Giấy Phép

Dự án này được cấp phép theo [Apache License 2.0](https://www.apache.org/licenses/LICENSE-2.0).

## Ghi Công
* Người bảo trì: [chunix64](https://github.com/chunix64)
* Lấy cảm hứng từ [Universal Arduino Telegram Bot](https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot)
* Được xây dựng cho các dự án IoT dùng Arduino & ESP
