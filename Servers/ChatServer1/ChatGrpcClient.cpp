#include "ChatGrpcClient.h"
#include "RedisMgr.h"
#include "ConfigMgr.h"
#include "UserMgr.h"
#include "CSession.h"
#include "MysqlMgr.h"
#include "ConfigMgr.h"
#include "Const.h"

ChatGrpcConPool::ChatGrpcConPool(size_t poolSize, std::string host, std::string port)
    : m_poolSize(poolSize), m_host(host), m_port(port), m_stop(false)
{
    for (size_t i = 0; i < m_poolSize; ++i)
    {

        std::shared_ptr<Channel> channel = grpc::CreateChannel(host + ":" + port, grpc::InsecureChannelCredentials());
        m_connections.push(ChatService::NewStub(channel));
    }
}

ChatGrpcConPool::~ChatGrpcConPool()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    Close();
    while (!m_connections.empty())
    {
        m_connections.pop();
    }
}

std::unique_ptr<ChatService::Stub> ChatGrpcConPool::getConnection()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    m_cond.wait(lock, [this]
                {
			if (m_stop) {
				return true;
			}
			return !m_connections.empty(); });
    // 如果停止则直接返回空指针
    if (m_stop)
    {
        return nullptr;
    }
    auto con = std::move(m_connections.front());
    m_connections.pop();
    return con;
}

void ChatGrpcConPool::returnConnection(std::unique_ptr<ChatService::Stub> con)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_stop)
    {
        return;
    }
    m_connections.push(std::move(con));
    m_cond.notify_one();
}

void ChatGrpcConPool::Close()
{
    m_stop = true;
    m_cond.notify_all();
}

ChatGrpcClient::~ChatGrpcClient()
{
}

AddFriendRsp ChatGrpcClient::NotifyAddFriend(std::string server_ip, const AddFriendReq &req)
{
    AddFriendRsp rsp;
    Defer defer([&rsp, &req]()
                {
		rsp.set_error(ErrorCodes::Success);
		rsp.set_applyuid(req.applyuid());
		rsp.set_touid(req.touid()); });

    auto find_iter = m_pools.find(server_ip);
    if (find_iter == m_pools.end())
    {
        return rsp;
    }

    auto &pool = find_iter->second;
    ClientContext context;
    auto stub = pool->getConnection();
    Status status = stub->NotifyAddFriend(&context, req, &rsp);
    Defer defercon([&stub, this, &pool]()
                   { pool->returnConnection(std::move(stub)); });

    if (!status.ok())
    {
        rsp.set_error(ErrorCodes::RPCFailed);
        return rsp;
    }

    return rsp;
}

AuthFriendRsp ChatGrpcClient::NotifyAuthFriend(std::string server_ip, const AuthFriendReq &req)
{
    AuthFriendRsp rsp;
    rsp.set_error(ErrorCodes::Success);

    Defer defer([&rsp, &req]()
                {
		rsp.set_fromuid(req.fromuid());
		rsp.set_touid(req.touid()); });

    auto find_iter = m_pools.find(server_ip);
    if (find_iter == m_pools.end())
    {
        return rsp;
    }

    auto &pool = find_iter->second;
    ClientContext context;
    auto stub = pool->getConnection();
    Status status = stub->NotifyAuthFriend(&context, req, &rsp);
    Defer defercon([&stub, this, &pool]()
                   { pool->returnConnection(std::move(stub)); });

    if (!status.ok())
    {
        rsp.set_error(ErrorCodes::RPCFailed);
        return rsp;
    }

    return rsp;
}

TextChatMsgRsp ChatGrpcClient::NotifyTextChatMsg(std::string server_ip,
                                                 const TextChatMsgReq &req, const Json::Value &rtvalue)
{

    TextChatMsgRsp rsp;
    rsp.set_error(ErrorCodes::Success);

    Defer defer([&rsp, &req]()
                {
                    rsp.set_fromuid(req.fromuid());
                    rsp.set_touid(req.touid());
                    for (const auto &text_data : req.textmsgs())
                    {
                        TextChatData *new_msg = rsp.add_textmsgs();
                        new_msg->set_msgid(text_data.msgid());
                        new_msg->set_msgcontent(text_data.msgcontent());
                    } });

    auto find_iter = m_pools.find(server_ip);
    if (find_iter == m_pools.end())
    {
        return rsp;
    }

    auto &pool = find_iter->second;
    ClientContext context;
    auto stub = pool->getConnection();
    Status status = stub->NotifyTextChatMsg(&context, req, &rsp);
    Defer defercon([&stub, this, &pool]()
                   { pool->returnConnection(std::move(stub)); });

    if (!status.ok())
    {
        rsp.set_error(ErrorCodes::RPCFailed);
        return rsp;
    }

    return rsp;
}

ChatGrpcClient::ChatGrpcClient()
{
    auto &cfg = ConfigMgr::Inst();
    auto server_list = cfg["PeerServer"]["Servers"];

    std::vector<std::string> words;

    std::stringstream ss(server_list);
    std::string word;

    while (std::getline(ss, word, ','))
    {
        words.push_back(word);
    }

    for (auto &word : words)
    {
        if (cfg[word]["Name"].empty())
        {
            continue;
        }
        m_pools[cfg[word]["Name"]] = std::make_unique<ChatGrpcConPool>(5, cfg[word]["Host"], cfg[word]["Port"]);
    }
}