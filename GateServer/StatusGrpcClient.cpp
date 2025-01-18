#include "StatusGrpcClient.h"

StatusGrpcConPool::StatusGrpcConPool(size_t poolSize, std::string host, std::string port)
    : m_poolSize(poolSize), m_host(host), m_port(port), m_stop(false)
{
    for (size_t i = 0; i < m_poolSize; ++i)
    {
        // 根据连接池大小，创建等量的连接加入队列
        std::shared_ptr<Channel> channel = grpc::CreateChannel(host + ":" + port, grpc::InsecureChannelCredentials());
        m_connections.push(StatusService::NewStub(channel));
    }
}

StatusGrpcConPool::~StatusGrpcConPool()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    Close();
    while (!m_connections.empty())
    {
        m_connections.pop();
    }
}

std::unique_ptr<StatusService::Stub> StatusGrpcConPool::getConnection()
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

void StatusGrpcConPool::returnConnection(std::unique_ptr<StatusService::Stub> con)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_stop)
    {
        return;
    }
    m_connections.push(std::move(con));
    m_cond.notify_one();
}

void StatusGrpcConPool::Close()
{
    m_stop = true;
    m_cond.notify_all();
}

StatusGrpcClient::~StatusGrpcClient()
{
}

GetChatServerRsp StatusGrpcClient::GetChatServer(int uid)
{
    ClientContext context;
    GetChatServerRsp reply;
    GetChatServerReq request;
    request.set_uid(uid);
    auto stub = m_pool->getConnection();
    Status status = stub->GetChatServer(&context, request, &reply);
    Defer defer([&stub, this]()
                { m_pool->returnConnection(std::move(stub)); });
    if (status.ok())
    {
        return reply;
    }
    else
    {
        reply.set_error(ErrorCodes::RPCFailed);
        return reply;
    }
}

LoginRsp StatusGrpcClient::Login(int uid, std::string token)
{
    ClientContext context;
    LoginRsp reply;
    LoginReq request;
    request.set_uid(uid);
    request.set_token(token);
    auto stub = m_pool->getConnection();
    Status status = stub->Login(&context, request, &reply);
    Defer defer([&stub, this]()
                { m_pool->returnConnection(std::move(stub)); });
    if (status.ok())
    {
        return reply;
    }
    else
    {
        reply.set_error(ErrorCodes::RPCFailed);
        return reply;
    }
}

StatusGrpcClient::StatusGrpcClient()
{
    auto &gCfgMgr = ConfigMgr::Inst();
    std::string host = gCfgMgr["StatusServer"]["Host"];
    std::string port = gCfgMgr["StatusServer"]["Port"];
    m_pool.reset(new StatusGrpcConPool(5, host, port));
}
