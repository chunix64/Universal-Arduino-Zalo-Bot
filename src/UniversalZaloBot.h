/*
  UniversalZaloBot - Library for Zalo bot.
  Created by chunix64, April 18, 2026.
  Licensed under the GNU GPL v3.
 */

#ifndef UNIVERSAL_ZALO_BOT_H
#define UNIVERSAL_ZALO_BOT_H
#define ZALO_DEBUG

#include <Arduino.h>
#include <ArduinoJson.h>
#include <Client.h>

#ifdef ESP32
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#define HAS_FREERTOS 1
#endif // ESP32

struct HttpResponse {
  String header;
  String body;
};

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

private:
  Client *client;
  String _apiHost;
  String _token;
  bool _isFreeRTOS;
#ifdef HAS_FREERTOS
  SemaphoreHandle_t _clientMutex;
#endif
  int _longPollTimeout;
  int _httpTimeout;
  int _maxMessageLength;
  void _yield();
  bool _ensureConnection(const String &host, int port);
  void _cleanupConnection();
  HttpResponse _get(const String &host, const String &slug = "/",
                    int port = 443);
  HttpResponse _post(const String &host, const String &slug = "/",
                     int port = 443, const String &payload = "");
  HttpResponse _parseHttpResponse(bool isPolling = false);
  bool _checkZaloRequestSuccess(const String &payload);
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
