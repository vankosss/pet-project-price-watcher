#pragma once
#include <string>
#include <drogon/HttpClient.h>
#include <cstdint>

class TelegramService {
private:
std::string getToken();

public:
  void sendMessage(int64_t id, const std::string& message);
};
