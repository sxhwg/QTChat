#pragma once
#include <unordered_map>
#include <string>
#include "Const.h"

/* http连接类，用于处理http连接 */
class HttpConnection : public std::enable_shared_from_this<HttpConnection>
{
public:
    HttpConnection(asio::io_context &ioc);
    void Start();
    void PreParseGetParam();
    tcp::socket &GetSocket();

private:
    friend class LogicSystem;
    void CheckDeadline();
    void WriteResponse();
    void HandleReq();

    tcp::socket m_socket;
    beast::flat_buffer m_buffer{8192};
    http::request<http::dynamic_body> m_request;
    http::response<http::dynamic_body> m_response;
    asio::steady_timer m_deadline{m_socket.get_executor(), std::chrono::seconds(60)};
    std::string m_get_url;
    std::unordered_map<std::string, std::string> m_get_params;
};
