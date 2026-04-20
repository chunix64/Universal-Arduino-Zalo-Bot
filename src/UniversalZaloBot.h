/*
  UniversalZaloBot - Library for Zalo bot.
  Created by chunix64, April 18, 2026.
  Licensed under the GNU GPL v3.
 */

#ifndef UNIVERSAL_ZALO_BOT_H
#define UNIVERSAL_ZALO_BOT_H
// #define ZALO_DEBUG

#include <Arduino.h>
#include <ArduinoJson.h>
#include <Client.h>

#ifdef ESP32
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#define HAS_FREERTOS 1
#endif // ESP32

enum MessageType {
  MESSAGE_UNKNOWN,
  MESSAGE_TEXT,
  MESSAGE_PHOTO,
  MESSAGE_STICKER,
};

struct Message {
  String chatId;
  String userId;
  String userName;
  String content;
  MessageType type = MESSAGE_UNKNOWN;
};

#if defined(ESP32) || defined(ESP8266)
#include <functional>
typedef std::function<void(const Message &)> ZaloEventCallback;
#else
typedef void (*ZaloEventCallback)(const Message &);
#endif // defined(ESP32) || defined(ESP8266)

class UniversalZaloBot {
public:
  UniversalZaloBot(const String &token, Client &client,
                   bool isFreeRTOS = false);
  void begin();
  void setApiHost(const String &host);
  String getApiHost();
  void setToken(const String &token);
  String getToken();
  void setLongPollTimeout(int longPollTimeout);
  int getLongPollTimeout();
  void setHttpTimeout(int httpTimeout);
  int getHttpTimeout();
  void setMaxMessageLength(int maxMessageLength);
  int getMaxMessageLength();
  String getApiBaseSlug();

  bool isTokenValid();
  String getBotName();
  bool sendMessage(const String &chat_id, const String &message);
  bool sendPhoto(const String &chat_id, const String &photo_url,
                 const String &caption = "");
  bool sendSticker(const String &chat_id, const String &sticker_id);
  bool sendChatAction(const String &chat_id, const String &action);
  Message getUpdates();

  void handleUpdate();

  void onText(ZaloEventCallback callback);
  void onPhoto(ZaloEventCallback callback);
  void onSticker(ZaloEventCallback callback);
  void onUpdate(ZaloEventCallback callback);
#if defined(ESP32) || defined(ESP8266)
  void onCommand(const String &command, ZaloEventCallback callback);
#endif // defined(ESP32) || defined(ESP8266)

private:
  struct HttpResponse {
    String header;
    String body;
  };

  struct ObserverNode {
    ZaloEventCallback callback;
    ObserverNode *next;
  };

  ObserverNode *_textObservers = nullptr;
  ObserverNode *_photoObservers = nullptr;
  ObserverNode *_stickerObservers = nullptr;
  ObserverNode *_updateObservers = nullptr;

#ifdef HAS_FREERTOS
  SemaphoreHandle_t _clientMutex;
#endif

  Client *client;
  String _apiHost;
  String _token;
  bool _isFreeRTOS;
  int _longPollTimeout;
  int _httpTimeout;
  int _maxMessageLength;
  void _yield();
  bool _ensureConnection(const String &host, int port);
  void _cleanupConnection();
  bool _checkForOkResponse(const String &payload);

  HttpResponse _get(const String &host, const String &slug = "/",
                    int port = 443, bool isPolling = false);
  HttpResponse _post(const String &host, const String &slug = "/",
                     int port = 443, const String &payload = "",
                     bool isPolling = false);
  HttpResponse _parseHttpResponse(bool isPolling = false);

  void _registerObserver(ObserverNode **head, ZaloEventCallback callback);
  void _notifyObservers(ObserverNode *head, const Message &message);
};

#ifdef HAS_FREERTOS
class MutexGuard {
public:
  MutexGuard(SemaphoreHandle_t &mutex) : _mutex(mutex), _taken(false) {
    if (xSemaphoreTake(_mutex, portMAX_DELAY) == pdTRUE) {
      _taken = true;
    }
  }

  ~MutexGuard() {
    if (_taken) {
      xSemaphoreGive(_mutex);
    }
  }

private:
  SemaphoreHandle_t &_mutex;
  bool _taken;
};
#endif // HAS_FREERTOS

#endif // UNIVERSAL_ZALO_BOT_H
