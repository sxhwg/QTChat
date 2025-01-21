#pragma once
#include <map>
#include <functional>
#include "Singleton.h"

class HttpConnection;
using HttpHandler = std::function<void(std::shared_ptr<HttpConnection>)>;

/* 逻辑类，用于处理http请求的逻辑 */
class LogicSystem : public Singleton<LogicSystem>
{
public:
    ~LogicSystem();
    void RegGet(std::string, HttpHandler handler);
    void RegPost(std::string, HttpHandler handler);
    bool HandleGet(std::string, std::shared_ptr<HttpConnection>);
    bool HandlePost(std::string, std::shared_ptr<HttpConnection>);

private:
    friend class Singleton<LogicSystem>;
    LogicSystem();
    std::map<std::string, HttpHandler> m_post_handlers;
    std::map<std::string, HttpHandler> m_get_handlers;
};
