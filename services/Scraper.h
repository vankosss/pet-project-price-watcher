#pragma once
#include <string>
#include <drogon/HttpTypes.h>
#include <drogon/HttpClient.h>

class Scraper {
public:
	void fetchHtml(const std::string& url, std::function<void(std::string s)> &&callback);
	double extractPrice(const std::string& html, const std::string& siteType);
};
