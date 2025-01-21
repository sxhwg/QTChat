#include <iostream>
#include "CServer.h"
#include "HttpConnection.h"
#include "AsioIOServicePool.h"

CServer::CServer(boost::asio::io_context &ioc, uint16_t &port) : m_ioc(ioc),
														  m_acceptor(ioc, tcp::endpoint(tcp::v4(), port))
{
}

void CServer::Start()
{
	// 获取指向自身的智能指针，并增加引用计数，构造伪闭包
	auto self = shared_from_this();

	// 获取io_context连接池中的io_context对象
	auto &io_context = AsioIOServicePool::GetInstance()->GetIOService();

	// 使用获取的io_context对象初始化http连接对象
	std::shared_ptr<HttpConnection> new_con = std::make_shared<HttpConnection>(io_context);

	// 异步接收连接，使用http连接对象中的socket与客户端通信
	m_acceptor.async_accept(new_con->GetSocket(), [self, new_con](boost::beast::error_code ec)
							{
	try {
		//出错则放弃这个连接，继续接收连接
		if (ec) {
			self->Start();
			return;
		}

		//接收成功，开始处理连接
		new_con->Start();

		self->Start();
	}catch (std::exception& exp) {
		std::cout << "exception is " << exp.what() << std::endl;
		self->Start();
	} });
}
