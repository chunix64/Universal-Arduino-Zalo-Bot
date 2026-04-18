#include <Arduino.h>
#include <Client.h>
#include "UniversalZaloBot.h"

UniversalZaloBot::UniversalZaloBot(const String& token, Client& client) {
  _token = token;
  _client = client;
}
