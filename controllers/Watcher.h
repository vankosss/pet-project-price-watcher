#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class Watcher : public drogon::HttpController<Watcher>
{
  public:
      METHOD_LIST_BEGIN
      ADD_METHOD_TO(Watcher::addUrl, "/watch", Post);
      METHOD_LIST_END

          void addUrl(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback);

};
