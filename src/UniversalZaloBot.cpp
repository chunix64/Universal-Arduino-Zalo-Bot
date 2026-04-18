#include "UniversalZaloBot.h"

UniversalZaloBot::UniversalZaloBot(const String &token, Client &client) {
  setToken(token);
  setApiHost("bot-api.zaloplatforms.com");
  setLongPoll(0);
  setHttpTimeout(1500);
  setMaxMessageLength(1500);
  this->client = &client;
}

void UniversalZaloBot::setApiHost(const String &host) {
  _apiHost = host;
}

String UniversalZaloBot::getApiHost() {
  return _apiHost;
}

void UniversalZaloBot::setToken(const String &token) {
  _token = token;
}

String UniversalZaloBot::getToken() {
  return _token;
}

void UniversalZaloBot::setLongPoll(int longPoll) {
  _longPoll = longPoll;
}

int UniversalZaloBot::getLongPoll() {
  return _longPoll;
}

void UniversalZaloBot::setHttpTimeout(int httpTimeout) {
  _httpTimeout = httpTimeout;
}

int UniversalZaloBot::getHttpTimeout() {
  return _httpTimeout;
};

void UniversalZaloBot::setMaxMessageLength(int maxMessageLength) {
  _maxMessageLength = maxMessageLength;
}

int UniversalZaloBot::getMaxMessageLength() {
  return _maxMessageLength;
}

String UniversalZaloBot::getApiBaseSlug() {
  return "/bot" + getToken() + "/";
}

bool UniversalZaloBot::sendMessage(const String &chat_id, const String &message) {
  String apiSlug = getApiBaseSlug() + "sendMessage";
  StaticJsonDocument<256> doc;
  doc["chat_id"] = chat_id;
  doc["text"] = message;

  String payload;
  serializeJson(doc, payload);

  HttpResponse res = _post(getApiHost(), apiSlug, 443, payload);
  return res.body.length() > 0;
}

// Private
HttpResponse UniversalZaloBot::_get(const String &host, const String &slug, int port) {
  HttpResponse httpResponse;
  if (!client->connected() || _currentHost != host) {
#ifdef ZALO_DEBUG
    Serial.println(F("[ZALO] Connecting to server"));
#endif // ZALO_DEBUG

    if (!client->connect(host.c_str(), port)) {
#ifdef ZALO_DEBUG
      Serial.println(F("[ZALO] Connection error"));
#endif // ZALO_DEBUG
      return httpResponse;
    }

    _currentHost = host;
  }

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
  }

  return _parseHttpResponse();
}

HttpResponse UniversalZaloBot::_post(const String &host, const String& slug, int port, const String &payload) {
  HttpResponse httpResponse;
  if (!client->connected() || _currentHost != host) {
#ifdef ZALO_DEBUG
    Serial.println(F("[ZALO] Connecting to server"));
#endif // ZALO_DEBUG

    if (!client->connect(host.c_str(), port)) {
#ifdef ZALO_DEBUG
      Serial.println(F("[ZALO] Connection error"));
#endif // ZALO_DEBUG
      return httpResponse;
    }

    _currentHost = host;
  }

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
    Serial.println(payloadJson);
#endif // ZALO_DEBUG
  }

  return _parseHttpResponse();
}

HttpResponse UniversalZaloBot::_parseHttpResponse() {
  HttpResponse httpResponse;

  int character_count = 0;
  unsigned long now = millis();
  bool finishedHeaders = false;
  bool currentLineIsBlank = true;
  bool responseReceived = false;

  while (millis() - now < getLongPoll() + getHttpTimeout()) {
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

      if (currentCharacter == '\n') currentLineIsBlank = true;
      else if (currentCharacter != '\r') currentLineIsBlank = false;
    }
  }

  return httpResponse;
}
