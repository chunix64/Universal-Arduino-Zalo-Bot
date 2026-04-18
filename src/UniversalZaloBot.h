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

struct HttpResponse {
  String header;
  String body;
};

class UniversalZaloBot {
public:
  UniversalZaloBot(const String &token, Client &client);
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

private:
  Client *client;
  String _apiHost;
  String _token;
  String _currentHost;
  int _longPollTimeout;
  int _httpTimeout;
  int _maxMessageLength;
  bool _ensureConnected(const String &host, int port);
  HttpResponse _get(const String &host, const String &slug = "/",
                    int port = 443);
  HttpResponse _post(const String &host, const String &slug = "/",
                     int port = 443, const String &payload = "");
  HttpResponse _parseHttpResponse();
};

#endif
