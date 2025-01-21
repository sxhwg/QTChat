#include "CServer.h"
#include <iostream>
#include "AsioIOServicePool.h"
#include "UserMgr.h"
CServer::CServer(boost::asio::io_context &io_context, short port) : m_io_context(io_context), m_port(port),
                                                                    m_acceptor(io_context, tcp::endpoint(tcp::v4(), port))
{
    std::cout << "Server start success, listen on port : " << m_port << std::endl;
    StartAccept();
}

CServer::~CServer()
{
    std::cout << "Server destruct listen on port : " << m_port << std::endl;
}

void CServer::ClearSession(std::string uuid)
{

    if (m_sessions.find(uuid) != m_sessions.end())
    {
        // 移除用户和session的关联
        UserMgr::GetInstance()->RmvUserSession(m_sessions[uuid]->GetUserId());
    }

    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_sessions.erase(uuid);
    }
}

void CServer::StartAccept()
{
    auto &io_context = AsioIOServicePool::GetInstance()->GetIOService();
    std::shared_ptr<CSession> new_session = std::make_shared<CSession>(io_context, this);
    m_acceptor.async_accept(new_session->GetSocket(), std::bind(&CServer::HandleAccept, this, new_session, std::placeholders::_1));
}

void CServer::HandleAccept(std::shared_ptr<CSession> new_session, const boost::system::error_code &error)
{
    if (!error)
    {
        new_session->Start();
        std::lock_guard<std::mutex> lock(m_mutex);
        m_sessions.insert(make_pair(new_session->GetSessionId(), new_session));
    }
    else
    {
        std::cout << "session accept failed, error is " << error.what() << std::endl;
    }

    StartAccept();
}