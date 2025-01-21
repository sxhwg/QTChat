#pragma once
#include <grpcpp/grpcpp.h>
#include "message.grpc.pb.h"
#include <mutex>

using grpc::ServerContext;
using grpc::Status;

using message::GetChatServerReq;
using message::GetChatServerRsp;
using message::LoginReq;
using message::LoginRsp;
using message::StatusService;

class ChatServer
{
public:
    ChatServer();
    ChatServer(const ChatServer &cs);
    ChatServer &operator=(const ChatServer &cs);

    std::string host;
    std::string port;
    std::string name;
    int con_count;
};
class StatusServiceImpl final : public StatusService::Service
{
public:
    StatusServiceImpl();
    Status GetChatServer(ServerContext *context, const GetChatServerReq *request, GetChatServerRsp *reply) override;
    Status Login(ServerContext *context, const LoginReq *request, LoginRsp *reply) override;

private:
    void insertToken(int uid, std::string token);
    ChatServer getChatServer();
    std::string generate_unique_string();
    std::unordered_map<std::string, ChatServer> m_servers;
    std::mutex m_server_mtx;
};