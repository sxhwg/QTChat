#pragma once
#include <grpcpp/grpcpp.h>
#include <atomic>
#include <memory>
#include <queue>
#include <condition_variable>
#include "Singleton.h"
#include "message.grpc.pb.h"
#include "message.pb.h"


using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using message::GetChatServerReq;
using message::GetChatServerRsp;
using message::LoginReq;
using message::LoginRsp;
using message::StatusService;

/* RPC连接池，用于提高并发性能 */
class StatusGrpcConPool
{
public:
    StatusGrpcConPool(size_t poolSize, std::string host, std::string port);
    ~StatusGrpcConPool();

    std::unique_ptr<StatusService::Stub> getConnection();
    void returnConnection(std::unique_ptr<StatusService::Stub> con);
    void Close();

private:
    std::atomic<bool> m_stop;
    size_t m_poolSize;
    std::string m_host;
    std::string m_port;
    std::queue<std::unique_ptr<StatusService::Stub>> m_connections;
    std::mutex m_mutex;
    std::condition_variable m_cond;
};

/* grpc客户端类，用于grpc调用 */
class StatusGrpcClient : public Singleton<StatusGrpcClient>
{
public:
    ~StatusGrpcClient();
    GetChatServerRsp GetChatServer(int uid);
    LoginRsp Login(int uid, std::string token);

private:
    friend class Singleton<StatusGrpcClient>;
    StatusGrpcClient();
    std::unique_ptr<StatusGrpcConPool> m_pool;
};