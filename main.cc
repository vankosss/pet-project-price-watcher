#include <drogon/drogon.h>
#include <drogon/orm/Mapper.h>
#include <drogon/orm/Criteria.h>
#include "models/Products.h"
#include <vector>
#include <iostream>
#include "services/Scraper.h"
#include <services/TelegramService.h>

void checkAllProduct() {
    LOG_INFO << "Starting check products";
    auto dbClient = drogon::app().getDbClient();
    drogon::orm::Mapper <drogon_model::price_watcher::Products> mapper(dbClient);
    
    mapper.findBy(
        drogon::orm::Criteria("is_active", drogon::orm::CompareOperator::EQ, true),
        [](const std::vector<drogon_model::price_watcher::Products>& products) {
            auto scraper = std::make_shared<Scraper>();
            LOG_INFO << "Products find: " << products.size();
            for (const auto& product : products) {
                std::string url = product.getValueOfUrl();
                std::string targetPrice = product.getValueOfTargetPrice();

                LOG_INFO << "Product ID: " << product.getValueOfId()
                    << " URL: " << url
                    << " Waiting price: " << targetPrice;

                scraper->fetchHtml(url, [product, targetPrice, scraper](std::string html) {
                    if (html.empty()) {
                        LOG_ERROR << "Can't upload page for Product ID: " << product.getValueOfId();
                    }

                    LOG_INFO << "HTML received! Length: " << html.length()
                        << " | Snippet: " << html.substr(0, 1000);

                    double currentPrice = scraper->extractPrice(html, "default");
                    double targetPriceDouble = std::stod(targetPrice);
                    if (currentPrice > 0) {
                        LOG_INFO << "Product: " << product.getValueOfId()
                            << " Current price: " << currentPrice
                            << " Target price: " << targetPrice;

                        if (currentPrice <= targetPriceDouble) {
                            LOG_INFO << "Goal achieved for Product ID: " << product.getValueOfId();
                            TelegramService tg;
                            std::string message = "Price is down!\n" "Product ID: " + std::to_string(product.getValueOfId()) + "\n" "Current Price: " + std::to_string(currentPrice) + "\n" "URL on product: " + product.getValueOfUrl();
                            tg.sendMessage(product.getValueOfChatId(), message);
                        }
                    }
                    else {
                        LOG_WARN << "Can't find price in page for Product ID: " << product.getValueOfId();
                    }

                });
            }
        },
        [](const drogon::orm::DrogonDbException& e) {
            LOG_ERROR << "Database Error: " << e.base().what();
        }
    );
}

int main() {
    try {
        drogon::app().loadConfigFile("./config.json");

        drogon::app().getLoop()->runEvery(60.0, []() {
            checkAllProduct();
        });

        LOG_INFO << "Server is stared";
        drogon::app().run();
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
