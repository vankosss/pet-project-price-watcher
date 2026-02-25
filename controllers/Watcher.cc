#include "Watcher.h"
#include "Products.h"
#include <drogon/orm/Mapper.h>
#include <nlohmann/json.hpp> 

using namespace drogon_model::price_watcher;
using namespace drogon::orm;

void Watcher::addUrl(const HttpRequestPtr &req,
                     std::function<void(const HttpResponsePtr &)> &&callback)
{
    auto jsonPtr = req->getJsonObject();

    if (!jsonPtr) {
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(k400BadRequest);
        resp->setBody("Missing JSON body");
        callback(resp);
        return;
    }

    const auto &json = *jsonPtr;

    if (!json.isMember("url") || !json.isMember("target_price") || !json.isMember("chat_id")) {
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(k400BadRequest);
        resp->setBody("Missing required fields: url, target_price, or chat_id");
        callback(resp);
        return;
    }

    try {
        std::string url = json["url"].asString();
        std::string targetPrice = std::to_string(json["target_price"].asDouble());
        int64_t chatId = json["chat_id"].asInt64();

        Products p;
        p.setUrl(url);
        p.setTargetPrice(targetPrice); 
        p.setChatId(chatId);
        p.setIsActive(true);

        auto dbClient = drogon::app().getDbClient();
        Mapper<Products> mapper(dbClient);

        mapper.insert(p, [callback](const Products &savedProduct) {
            nlohmann::json resJson;
            resJson["status"] = "ok";
            resJson["id"] = savedProduct.getValueOfId();
            resJson["message"] = "Product added successfully";

            auto resp = HttpResponse::newHttpJsonResponse(resJson.dump());
            callback(resp);
        }, [callback](const DrogonDbException &e) {
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k500InternalServerError);
            resp->setBody("Database Error: " + std::string(e.base().what()));
            callback(resp);
        });

    } catch (const std::exception &e) {
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(k500InternalServerError);
        resp->setBody("Internal Error: " + std::string(e.what()));
        callback(resp);
    }
}