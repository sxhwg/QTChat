#pragma once
#include <grpcpp/grpcpp.h>
#include <queue>
#include "message.grpc.pb.h"
#include "message.pb.h"
#include "Singleton.h"
#include "ConfigMgr.h"
#include "Const.h"
#include "Data.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using message::AddFriendReq;
using message::AddFriendRsp;
using message::AuthFriendReq;
using message::AuthFriendRsp;
using message::ChatService;
using message::TextChatData;
using message::TextChatMsgReq;
using message::TextChatMsgRsp;

class ChatGrpcConPool
{
public:
    ChatGrpcConPool(size_t poolSize, std::string host, std::string port);
    ~ChatGrpcConPool();

    std::unique_ptr<ChatService::Stub> getConnection();
    void returnConnection(std::unique_ptr<ChatService::Stub> context);
    void Close();

private:
    std::atomic<bool> m_stop;
    size_t m_poolSize;
    std::string m_host;
    std::string m_port;
    std::queue<std::unique_ptr<ChatService::Stub>> m_connections;
    std::mutex m_mutex;
    std::condition_variable m_cond;
};

class ChatGrpcClient : public Singleton<ChatGrpcClient>
{
public:
    ~ChatGrpcClient();

    AddFriendRsp NotifyAddFriend(std::string server_ip, const AddFriendReq &req);
    AuthFriendRsp NotifyAuthFriend(std::string server_ip, const AuthFriendReq &req);
    TextChatMsgRsp NotifyTextChatMsg(std::string server_ip, const TextChatMsgReq &req, const Json::Value &rtvalue);

private:
    friend class Singleton<ChatGrpcClient>;
    ChatGrpcClient();
    std::unordered_map<std::string, std::unique_ptr<ChatGrpcConPool>> m_pools;
};