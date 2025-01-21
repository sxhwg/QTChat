#include "VerifyGrpcClient.h"
#include "Const.h"
#include "ConfigMgr.h"

VerifyGrpcConPool::VerifyGrpcConPool(size_t poolSize, std::string host, std::string port)
	: m_poolSize(poolSize), m_host(host), m_port(port), m_stop(false)
{
	for (size_t i = 0; i < m_poolSize; ++i)
	{
		// 根据连接池大小，创建等量的连接加入队列
		std::shared_ptr<Channel> channel = grpc::CreateChannel(host + ":" + port, grpc::InsecureChannelCredentials());
		m_connections.emplace(VerifyService::NewStub(channel));
	}
}

VerifyGrpcConPool::~VerifyGrpcConPool()
{
	std::lock_guard<std::mutex> lock(m_mutex);
	Close();
	while (!m_connections.empty())
	{
		m_connections.pop();
	}
}

std::unique_ptr<VerifyService::Stub> VerifyGrpcConPool::getConnection()
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

void VerifyGrpcConPool::returnConnection(std::unique_ptr<VerifyService::Stub> con)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	if (m_stop)
	{
		return;
	}
	m_connections.push(std::move(con));
	m_cond.notify_one();
}

void VerifyGrpcConPool::Close()
{
	m_stop = true;
	m_cond.notify_all();
}

VerifyGrpcClient::~VerifyGrpcClient()
{
}

GetVerifyRsp VerifyGrpcClient::GetVerifyCode(std::string email)
{
	ClientContext context;
	GetVerifyRsp reply;
	GetVerifyReq request;
	request.set_email(email);
	auto stub = m_pool->getConnection();
	Status status = stub->GetVerifyCode(&context, request, &reply);
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

VerifyGrpcClient::VerifyGrpcClient()
{
	auto &gCfgMgr = ConfigMgr::Inst();
	std::string host = gCfgMgr["VerifyServer"]["Host"];
	std::string port = gCfgMgr["VerifyServer"]["Port"];
	m_pool.reset(new VerifyGrpcConPool(5, host, port));
}