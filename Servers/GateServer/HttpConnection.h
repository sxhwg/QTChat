#pragma once
#include <unordered_map>
#include <string>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/beast/http.hpp>

using tcp = boost::asio::ip::tcp;
namespace beast = boost::beast;
namespace http = boost::beast::http;

/* http连接类，用于处理http连接 */
class HttpConnection : public std::enable_shared_from_this<HttpConnection>
{
public:
    HttpConnection(boost::asio::io_context &ioc);
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
    boost::asio::steady_timer m_deadline{m_socket.get_executor(), std::chrono::seconds(60)};
    std::string m_get_url;
    std::unordered_map<std::string, std::string> m_get_params;
};
