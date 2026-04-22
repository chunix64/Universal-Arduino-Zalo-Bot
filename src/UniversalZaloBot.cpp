#include "UniversalZaloBot.h"

UniversalZaloBot::UniversalZaloBot(const String &token, Client &client,
                                   bool isFreeRTOS) {
  setDebug(false);
  setToken(token);
  setApiHost("bot-api.zaloplatforms.com");
  setLongPollTimeout(30);
  setHttpTimeout(1500);
  setMaxMessageLength(1500);
  _isFreeRTOS = isFreeRTOS;

  this->client = &client;

#ifdef HAS_FREERTOS
  if (_isFreeRTOS) {
    if (_clientMutex == nullptr) {
      _clientMutex = xSemaphoreCreateMutex();
    }
  }
#endif
}

UniversalZaloBot::~UniversalZaloBot() {
  _freeObservers(_textObservers);
  _freeObservers(_photoObservers);
  _freeObservers(_stickerObservers);
  _freeObservers(_updateObservers);
}

void UniversalZaloBot::setDebug(bool isDebug) { _isDebug = isDebug; }

void UniversalZaloBot::begin() {
#ifndef HAS_FREERTOS
  if (_isFreeRTOS && _isDebug) {
    Serial.println("Your board does not support FreeRTOS");
  }
#endif // !HAS_FREERTOS
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

int UniversalZaloBot::getHttpTimeout() { return _httpTimeout; }

void UniversalZaloBot::setMaxMessageLength(int maxMessageLength) {
  _maxMessageLength = maxMessageLength;
}

int UniversalZaloBot::getMaxMessageLength() { return _maxMessageLength; }

String UniversalZaloBot::getApiBaseSlug() { return "/bot" + getToken() + "/"; }

bool UniversalZaloBot::isTokenValid() {
  String apiSlug = getApiBaseSlug() + "getMe";
  HttpResponse res = _post(getApiHost(), apiSlug);

  return _checkForOkResponse(res.body);
}

String UniversalZaloBot::getBotName() {
  String apiSlug = getApiBaseSlug() + "getMe";
  HttpResponse res = _post(getApiHost(), apiSlug);

  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, res.body);

  if (!error && doc["ok"]) {
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
  return _checkForOkResponse(res.body);
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
  return _checkForOkResponse(res.body);
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
  return _checkForOkResponse(res.body);
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
  return _checkForOkResponse(res.body);
}

Message UniversalZaloBot::getUpdates() {
  Message message;
  String apiSlug = getApiBaseSlug() + "getUpdates";
  StaticJsonDocument<256> reqDoc;
  reqDoc["timeout"] = getLongPollTimeout();

  String payload;
  serializeJson(reqDoc, payload);

  HttpResponse res = _post(getApiHost(), apiSlug, 443, payload, true);

  StaticJsonDocument<2048> resDoc;
  DeserializationError error = deserializeJson(resDoc, res.body);

  if (error || !resDoc["ok"]) {
    return message;
  }

  JsonObject result = resDoc["result"];

  if (result.isNull()) {
    return message;
  }

  JsonObject msg = result["message"];

  if (msg.isNull()) {
    return message;
  }

  const char *type = result["event_name"] | "";

  if (strcmp(type, "message.text.received") == 0) {
    message.type = MESSAGE_TEXT;
    message.content = msg["text"] | "";
  } else if (strcmp(type, "message.image.received") == 0) {
    message.type = MESSAGE_PHOTO;
    message.content = msg["photo_url"] | "";
  } else if (strcmp(type, "message.sticker.received") == 0) {
    message.type = MESSAGE_STICKER;
    message.content = msg["sticker"] | "";
  } else {
    message.type = MESSAGE_UNKNOWN;
    message.content = "";
  }

  message.chatId = msg["chat"]["id"] | "";
  message.userId = msg["from"]["id"] | "";
  message.userName = msg["from"]["display_name"] | "";

  return message;
}

void UniversalZaloBot::handleUpdate() {
  Message message = getUpdates();

  if (message.type == MESSAGE_UNKNOWN) {
    return;
  }

  _notifyObservers(_updateObservers, message);

  switch (message.type) {
  case MESSAGE_TEXT:
    _notifyObservers(_textObservers, message);
    break;
  case MESSAGE_PHOTO:
    _notifyObservers(_photoObservers, message);
    break;
  case MESSAGE_STICKER:
    _notifyObservers(_stickerObservers, message);
    break;
  default:
    break;
  }
}

void UniversalZaloBot::onText(ZaloEventCallback callback) {
  _registerObserver(&_textObservers, callback);
}

void UniversalZaloBot::onPhoto(ZaloEventCallback callback) {
  _registerObserver(&_photoObservers, callback);
}

void UniversalZaloBot::onSticker(ZaloEventCallback callback) {
  _registerObserver(&_stickerObservers, callback);
}

void UniversalZaloBot::onUpdate(ZaloEventCallback callback) {
  _registerObserver(&_updateObservers, callback);
}

#if defined(ESP32) || defined(ESP8266)
void UniversalZaloBot::onCommand(const String &command,
                                 ZaloEventCallback callback) {
  _registerObserver(&_textObservers,
                    [command, callback](const Message &message) {
                      if (message.type != MESSAGE_TEXT) {
                        return;
                      }

                      if (message.content.startsWith(command)) {
                        callback(message);
                      }
                    });
}
#endif // defined(ESP32) || defined(ESP8266)

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
    if (_isDebug) {
      Serial.print(F("[ZALO] Connecting to "));
      Serial.println(host);
    }

    if (!client->connect(host.c_str(), port)) {
      if (_isDebug) {
        Serial.println(F("[ZALO] Connection error"));
      }

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

  if (_isDebug) {
    Serial.println("[ZALO] Connection cleaned");
  }
}

bool UniversalZaloBot::_checkForOkResponse(const String &payload) {
  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, payload);

  return !error && doc["ok"];
}

UniversalZaloBot::HttpResponse
UniversalZaloBot::_post(const String &host, const String &slug, int port,
                        const String &payload, bool isPolling) {
  HttpResponse httpResponse;

#ifdef HAS_FREERTOS
  if (_isFreeRTOS) {
    MutexGuard guard(_clientMutex);
    httpResponse = _postInternal(host, slug, port, payload, isPolling);
    return httpResponse;
  }
#endif

  httpResponse = _postInternal(host, slug, port, payload, isPolling);
  return httpResponse;
}

UniversalZaloBot::HttpResponse
UniversalZaloBot::_postInternal(const String &host, const String &slug,
                                int port, const String &payload,
                                bool isPolling) {
  HttpResponse httpResponse;
  if (!_ensureConnection(host, port))
    return httpResponse;

  if (client->connected()) {
    if (_isDebug) {
      Serial.print(F("[ZALO] Connected to "));
      Serial.println(host);
    }

    client->print(F("POST "));
    client->print(slug.length() ? slug : "/");
    client->println(F(" HTTP/1.1"));
    client->print(F("Host:"));
    client->println(host);
    client->println(F("Content-Type: application/json"));
    client->println(F("Connection: keep-alive"));
    client->print(F("Content-Length:"));
    client->println(payload.length());
    client->println();
    client->println(payload);

    if (_isDebug) {
      Serial.print(F("[ZALO] POST: "));
      Serial.println(payload);
    }

    httpResponse = _parseHttpResponse(isPolling);
  }

  _cleanupConnection();
  return httpResponse;
}

UniversalZaloBot::HttpResponse
UniversalZaloBot::_parseHttpResponse(bool isPolling) {
  HttpResponse httpResponse;

  int character_count = 0;
  unsigned long now = millis();
  bool finishedHeaders = false;
  bool currentLineIsBlank = true;
  bool responseReceived = false;
  int currentHttpTimeout = getHttpTimeout();

  if (isPolling) {
    currentHttpTimeout += getLongPollTimeout() * 1000;
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

    if (responseReceived && !client->available()) {
      break;
    }

    _yield();
  }

  return httpResponse;
}

void UniversalZaloBot::_registerObserver(ObserverNode **head,
                                         ZaloEventCallback callback) {
  if (callback == nullptr) {
    return;
  }

  ObserverNode *node = new ObserverNode{callback, *head};
  *head = node;
}

void UniversalZaloBot::_notifyObservers(ObserverNode *head,
                                        const Message &message) {
  ObserverNode *current = head;

  while (current) {
    if (current->callback) {
      current->callback(message);
    }
    current = current->next;
  }
}

void UniversalZaloBot::_freeObservers(ObserverNode *head) {
  while (head) {
    ObserverNode *temp = head;
    head = head->next;
    delete temp;
  }
}

#ifdef HAS_FREERTOS
MutexGuard::MutexGuard(SemaphoreHandle_t &mutex)
    : _mutex(mutex), _taken(false) {
  if (xSemaphoreTake(_mutex, portMAX_DELAY) == pdTRUE) {
    _taken = true;
  }
}

MutexGuard::~MutexGuard() {
  if (_taken) {
    xSemaphoreGive(_mutex);
  }
}

void MutexGuard::unlock() {
  if (_taken) {
    xSemaphoreGive(_mutex);
    _taken = false;
  }
}

bool MutexGuard::lock() {
  if (!_taken) {
    if (xSemaphoreTake(_mutex, portMAX_DELAY) == pdTRUE) {
      _taken = true;
      return true;
    }
  }
  return false;
}
#endif // HAS_FREERTOS
