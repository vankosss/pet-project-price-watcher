#include "TelegramService.h"
#include <drogon/drogon.h>

std::string TelegramService::getToken() {
	return drogon::app().getCustomConfig()["telegram_token"].asString();
}

void TelegramService::sendMessage(int64_t id, const std::string& message) {
	std::string token = getToken();
	if (token.empty()) {
		LOG_ERROR << "Telegram token is missing";
		return;
	}

	auto client = drogon::HttpClient::newHttpClient("https://api.telegram.org");
	auto req = drogon::HttpRequest::newHttpRequest();
	req->setMethod(drogon::Post);
	req->setPath("/bot" + token + "/sendMessage");

	Json::Value body;
	body["chat_id"] = id;
	body["text"] = message;
	body["parse_mode"] = "HTML";

	req->setBody(body.toStyledString());
	req->setContentTypeCode(drogon::CT_APPLICATION_JSON);

	client->sendRequest(req, [id](drogon::ReqResult res, const drogon::HttpResponsePtr& resp) {
		if (res == drogon::ReqResult::Ok && resp->statusCode() == 200) {
			LOG_INFO << "Message sent to " << id;
		}
		else {
			LOG_ERROR << "Message don't send " << (resp ? resp->getBody() : "No response");
		}
	});
}