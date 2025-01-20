#pragma once
#include <queue>
#include <mutex>
#include <memory>
#include "Const.h"
#include "MsgNode.h"

class CServer;

class CSession : public std::enable_shared_from_this<CSession>
{
public:
    CSession(asio::io_context &io_context, CServer *server);
    ~CSession();

    void Start();
    void Close();
    std::shared_ptr<CSession> SharedSelf();
    tcp::socket &GetSocket();
    std::string &GetSessionId();
    void SetUserId(int uid);
    int GetUserId();

    void Send(char *msg, short max_length, short msgid);
    void Send(std::string msg, short msgid);
    void AsyncReadHead(int total_len);
    void AsyncReadBody(int length);

private:
    void asyncReadFull(std::size_t maxLength,
                       std::function<void(const boost::system::error_code &, std::size_t)> handler);
    void asyncReadLen(std::size_t read_len, std::size_t total_len,
                      std::function<void(const boost::system::error_code &, std::size_t)> handler);
    void HandleWrite(const boost::system::error_code &error, std::shared_ptr<CSession> shared_self);

    CServer *m_server;
    bool m_close;
    tcp::socket m_socket;
    std::string m_session_id;
    int m_user_uid;
    char m_data[MAX_LENGTH];
    std::mutex m_send_lock;
    std::queue<std::shared_ptr<SendNode>> m_send_que;
    std::shared_ptr<RecvNode> m_recv_msg_node;
    bool m_head_parse;
    std::shared_ptr<MsgNode> m_recv_head_node;
};

class LogicNode
{
public:
    LogicNode(std::shared_ptr<CSession>, std::shared_ptr<RecvNode>);

private:
    friend class LogicSystem;
    std::shared_ptr<CSession> m_session;
    std::shared_ptr<RecvNode> m_recvnode;
};