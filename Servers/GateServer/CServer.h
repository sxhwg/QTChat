#pragma once
#include <string>
#include <memory>
#include <boost/asio.hpp>

using tcp = boost::asio::ip::tcp;

/* 连接服务类，用于监听和接收连接 */
class CServer : public std::enable_shared_from_this<CServer>
{
public:
    CServer(boost::asio::io_context &ioc, uint16_t &port);
    void Start();

private:
    tcp::acceptor m_acceptor;
    boost::asio::io_context &m_ioc;
};