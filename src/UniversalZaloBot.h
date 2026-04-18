/*
  UniversalZaloBot - Library for Zalo bot.
  Created by chunix64, April 18, 2026.
  Licensed under the GNU GPL v3.
 */

#ifndef UniversalZaloBot_h
#define UniversalZaloBot_h

#include <Arduino.h>
#include <Client.h>

class UniversalZaloBot {
  public:
    UniversalZaloBot(const String& token, Client& client);
    sendMessage();
  private:
    String _token;
    Client client;
}

#endif
