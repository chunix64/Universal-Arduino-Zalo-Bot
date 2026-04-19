#include "UniversalZaloBot.h"

UniversalZaloBot::UniversalZaloBot(const String &token, Client &client,
                                   bool isFreeRTOS) {
  setToken(token);
  setApiHost("bot-api.zaloplatforms.com");
  setLongPollTimeout(0);
  setHttpTimeout(1500);
  setMaxMessageLength(1500);
  _isFreeRTOS = isFreeRTOS;

  this->client = &client;
}

void UniversalZaloBot::begin() {
#ifndef HAS_FREERTOS
#ifdef ZALO_DEBUG
  if (_isFreeRTOS) {
    Serial.println("Your board does not support FreeRTOS");
  }
#endif // ZALO_DEBUG
#endif // !HAS_FREERTOS

#ifdef HAS_FREERTOS
  if (_isFreeRTOS) {
    if (_clientMutex == nullptr) {
      _clientMutex = xSemaphoreCreateMutex();
    }
  }
#endif
}

void UniversalZaloBot::setApiHost(const String &host) { _apiHost = host; }

String UniversalZaloBot::getApiHost() { return _apiHost; }

void UniversalZaloBot::setToken(const String &token) { _token = token; }

String UniversalZaloBot::getToken() { return _token; }

void UniversalZaloBot::setLongPollTimeout(int longPollTimeout) {
  _longPollTimeout = longPollTimeout;
}

int UniversalZaloBot::getLongPollTimeout() { return _longPollTimeout; }

void UniversalZaloBot::setHttpTimeout(int httpTimeout) {
  _httpTimeout = httpTimeout;
}

int UniversalZaloBot::getHttpTimeout() { return _httpTimeout; };

void UniversalZaloBot::setMaxMessageLength(int maxMessageLength) {
  _maxMessageLength = maxMessageLength;
}

int UniversalZaloBot::getMaxMessageLength() { return _maxMessageLength; }

String UniversalZaloBot::getApiBaseSlug() { return "/bot" + getToken() + "/"; }

bool UniversalZaloBot::isTokenValid() {
  String apiSlug = getApiBaseSlug() + "getMe";
  HttpResponse res = _post(getApiHost(), apiSlug);

  return _checkZaloRequestSuccess(res.body);
}

String UniversalZaloBot::getBotName() {
  String apiSlug = getApiBaseSlug() + "getMe";
  HttpResponse res = _post(getApiHost(), apiSlug);

  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, res.body);

  if (!error && doc["error_code"] == 0) {
    return doc["result"]["display_name"];
  }

  return "Unknown";
}

bool UniversalZaloBot::sendMessage(const String &chat_id,
                                   const String &message) {
  String apiSlug = getApiBaseSlug() + "sendMessage";
  StaticJsonDocument<1024> doc;
  doc["chat_id"] = chat_id;
  doc["text"] = message;

  String payload;
  serializeJson(doc, payload);

  HttpResponse res = _post(getApiHost(), apiSlug, 443, payload);
  return _checkZaloRequestSuccess(res.body);
}

bool UniversalZaloBot::sendPhoto(const String &chat_id, const String &photo_url,
                                 const String &caption) {
  String apiSlug = getApiBaseSlug() + "sendPhoto";
  StaticJsonDocument<1024> doc;
  doc["chat_id"] = chat_id;
  doc["caption"] = caption;
  doc["photo"] = photo_url;

  String payload;
  serializeJson(doc, payload);

  HttpResponse res = _post(getApiHost(), apiSlug, 443, payload);
  return _checkZaloRequestSuccess(res.body);
}

bool UniversalZaloBot::sendSticker(const String &chat_id,
                                   const String &sticker_id) {
  String apiSlug = getApiBaseSlug() + "sendSticker";
  StaticJsonDocument<1024> doc;
  doc["chat_id"] = chat_id;
  doc["sticker"] = sticker_id;

  String payload;
  serializeJson(doc, payload);

  HttpResponse res = _post(getApiHost(), apiSlug, 443, payload);
  return _checkZaloRequestSuccess(res.body);
}

bool UniversalZaloBot::sendChatAction(const String &chat_id,
                                      const String &action) {
  String apiSlug = getApiBaseSlug() + "sendChatAction";
  StaticJsonDocument<1024> doc;
  doc["chat_id"] = chat_id;
  doc["action"] = action;

  String payload;
  serializeJson(doc, payload);

  HttpResponse res = _post(getApiHost(), apiSlug, 443, payload);
  return _checkZaloRequestSuccess(res.body);
}

//---------------------------------------------------------
//
// Private
//
//---------------------------------------------------------

void UniversalZaloBot::_yield() {
#ifdef HAS_FREERTOS
  if (_isFreeRTOS) {
    vTaskDelay(pdMS_TO_TICKS(1));
    return;
  }
#endif // HAS_FREERTOS

  delay(1);
}

bool UniversalZaloBot::_ensureConnection(const String &host, int port) {
  if (client->connected()) {
    _cleanupConnection();
  }

  if (!client->connected()) {
#ifdef ZALO_DEBUG
    Serial.print(F("[ZALO] Connecting to "));
    Serial.println(host);
#endif // ZALO_DEBUG

    if (!client->connect(host.c_str(), port)) {
#ifdef ZALO_DEBUG
      Serial.println(F("[ZALO] Connection error"));
#endif // ZALO_DEBUG
      return false;
    }
  }

  return true;
}

void UniversalZaloBot::_cleanupConnection() {
  if (client->connected()) {
    client->flush();
    client->stop();
  }

#ifdef ZALO_DEBUG
  Serial.println("[ZALO] Connection cleaned");
#endif // ZALO_DEBUG
}

HttpResponse UniversalZaloBot::_get(const String &host, const String &slug,
                                    int port) {
  HttpResponse httpResponse;

#ifdef HAS_FREERTOS
  if (_isFreeRTOS) {
    MutexGuard guard(_clientMutex);
  }
#endif

  if (!_ensureConnection(host, port))
    return httpResponse;

  if (client->connected()) {
#ifdef ZALO_DEBUG
    Serial.print(F("[ZALO] Connected to "));
    Serial.println(host);
#endif // ZALO_DEBUG

    client->print(F("GET "));
    client->print(slug.length() ? slug : "/");
    client->println(F(" HTTP/1.1"));
    client->print(F("Host:"));
    client->println(host);
    client->println(F("Accept: application/json"));
    client->println(F("Cache-Control: no-cache"));
    client->println();

#ifdef ZALO_DEBUG
    Serial.print(F("[ZALO] GET: "));
    Serial.println(slug.length() ? slug : "/");
#endif // ZALO_DEBUG

    httpResponse = _parseHttpResponse();
  }

  _cleanupConnection();
  return httpResponse;
}

HttpResponse UniversalZaloBot::_post(const String &host, const String &slug,
                                     int port, const String &payload) {
  HttpResponse httpResponse;

#ifdef HAS_FREERTOS
  if (_isFreeRTOS) {
    MutexGuard guard(_clientMutex);
  }
#endif

  if (!_ensureConnection(host, port))
    return httpResponse;

  if (client->connected()) {
#ifdef ZALO_DEBUG
    Serial.print(F("[ZALO] Connected to "));
    Serial.println(host);
#endif // ZALO_DEBUG

    client->print(F("POST "));
    client->print(slug.length() ? slug : "/");
    client->println(F(" HTTP/1.1"));
    client->print(F("Host:"));
    client->println(host);
    client->println(F("Content-Type: application/json"));
    client->print(F("Content-Length:"));
    client->println(payload.length());
    client->println();
    client->println(payload);

#ifdef ZALO_DEBUG
    Serial.print(F("[ZALO] POST: "));
    Serial.println(payload);
#endif // ZALO_DEBUG

    httpResponse = _parseHttpResponse();
  }

  _cleanupConnection();
  return httpResponse;
}

HttpResponse UniversalZaloBot::_parseHttpResponse(bool isPolling) {
  HttpResponse httpResponse;

  int character_count = 0;
  unsigned long now = millis();
  bool finishedHeaders = false;
  bool currentLineIsBlank = true;
  bool responseReceived = false;
  int currentHttpTimeout = getHttpTimeout();

  if (isPolling) {
    currentHttpTimeout += getLongPollTimeout();
  }

  while (millis() - now < currentHttpTimeout) {
    while (client->available()) {
      char currentCharacter = client->read();
      responseReceived = true;

      if (!finishedHeaders) {
        if (currentLineIsBlank && currentCharacter == '\n') {
          finishedHeaders = true;
        } else {
          httpResponse.header += currentCharacter;
        }
      } else {
        if (character_count < getMaxMessageLength()) {
          httpResponse.body += currentCharacter;
          character_count++;
        }
      }

      if (currentCharacter == '\n')
        currentLineIsBlank = true;
      else if (currentCharacter != '\r')
        currentLineIsBlank = false;
    }

    _yield();
  }

  return httpResponse;
}

bool UniversalZaloBot::_checkZaloRequestSuccess(const String &payload) {
  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, payload);

  return !error && doc["error_code"] == 0;
}
