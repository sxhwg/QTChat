#pragma once
#include <memory>
#include <map>
#include <mutex>
#include "CSession.h"
#include "Const.h"

class CServer
{
public:
	CServer(boost::asio::io_context &io_context, short port);
	~CServer();

	void ClearSession(std::string);

private:
	void StartAccept();
	void HandleAccept(std::shared_ptr<CSession>, const boost::system::error_code &error);

	boost::asio::io_context &m_io_context;
	short m_port;
	tcp::acceptor m_acceptor;
	std::map<std::string, std::shared_ptr<CSession>> m_sessions;
	std::mutex m_mutex;
};