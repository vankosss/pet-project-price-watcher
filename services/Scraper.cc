#include "Scraper.h"
#include <drogon/drogon.h>
#include <gumbo.h>
#include <iostream>
#include <regex>

void aggregateText(GumboNode* node, std::string& out) {
	if (node->type == GUMBO_NODE_TEXT) {
		out += node->v.text.text;
	}
	else if (node->type == GUMBO_NODE_ELEMENT) {
		GumboVector* children = &node->v.element.children;
		for (unsigned int i = 0; i < children->length; ++i) {
			aggregateText(static_cast<GumboNode*>(children->data[i]), out);
		}
	}
}

std::string findTextInTag(GumboNode* node, const std::string& target) {
	if (node->type != GUMBO_NODE_ELEMENT) {
		return "";
	}

	GumboAttribute* classAttr = gumbo_get_attribute(&node->v.element.attributes, "class");
	if (classAttr && std::string(classAttr->value).find(target) != std::string::npos) {
		GumboVector* children = &node->v.element.children;
		std::string text;
		aggregateText(node, text);
		return text;
	}

	GumboVector* children = &node->v.element.children;
	for (unsigned int i = 0; i < children->length; ++i) {
		std::string res = findTextInTag(static_cast<GumboNode*>(children->data[i]), target);
		if (!res.empty()) {
			return res;
		}
	}

	return "";
}

void Scraper::fetchHtml(const std::string& url, std::function<void(std::string s)>&& callback) {
	auto client = drogon::HttpClient::newHttpClient(url);
	auto req = drogon::HttpRequest::newHttpRequest();
	req->setMethod(drogon::Get);
	std::string path = "/";
	size_t pos = url.find("://");
	if (pos != std::string::npos) {
		size_t pathStart = url.find("/", pos + 3);
		if (pathStart != std::string::npos) {
			path = url.substr(pathStart);
		}
	}
	req->setPath(path);
	req->addHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36");
	client->sendRequest(req, [callback = std::move(callback)](drogon::ReqResult res, const drogon::HttpResponsePtr& resp) {
		if (res == drogon::ReqResult::Ok && resp) {
			callback(std::string(resp->getBody()));
		}
		else {
			LOG_ERROR << "Failed to fetch URL";
			callback("");
		}
	});
}

double Scraper::extractPrice(const std::string& html, const std::string& siteType) {
	if (html.empty()) return -1.0;
	GumboOutput* output = gumbo_parse(html.c_str());
	std::string rawPrice = findTextInTag(output->root, "price");
	gumbo_destroy_output(&kGumboDefaultOptions, output);
	if (rawPrice.empty()) return -1.0;
	std::string cleaned;
	for (unsigned char c : rawPrice) {
		if (std::isdigit(c) || c == ',' || c == '.') {
			cleaned += c;
		}
	}
	if (cleaned.empty()) return -1.0;
	std::replace(cleaned.begin(), cleaned.end(), ',', '.');
	std::regex RegexPrice(R"([0-9]+(\.[0-9]+)?)");
	std::smatch match;

	if (std::regex_search(cleaned, match, RegexPrice)) {
		try {
			return std::stod(match.str());
		}
		catch (...) {
			return -1.0;
		}
	}

	return -1.0;
}
