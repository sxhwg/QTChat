#pragma once
#include <grpcpp/grpcpp.h>
#include <atomic>
#include <memory>
#include <queue>
#include "Singleton.h"
#include "message.grpc.pb.h"
#include "message.pb.h"
#include "Const.h"
#include "ConfigMgr.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using message::GetVerifyReq;
using message::GetVerifyRsp;
using message::VerifyService;

/* RPC连接池，用于提高并发性能 */
class VerifyGrpcConPool
{
public:
	VerifyGrpcConPool(size_t poolSize, std::string host, std::string port);
	~VerifyGrpcConPool();

	std::unique_ptr<VerifyService::Stub> getConnection();
	void returnConnection(std::unique_ptr<VerifyService::Stub> con);
	void Close();

private:
	std::atomic<bool> m_stop;
	size_t m_poolSize;
	std::string m_host;
	std::string m_port;
	std::queue<std::unique_ptr<VerifyService::Stub>> m_connections;
	std::mutex m_mutex;
	std::condition_variable m_cond;
};

/* grpc客户端类，用于grpc调用 */
class VerifyGrpcClient : public Singleton<VerifyGrpcClient>
{
public:
	~VerifyGrpcClient();
	GetVerifyRsp GetVerifyCode(std::string email);

private:
	friend class Singleton<VerifyGrpcClient>;
	VerifyGrpcClient();

	std::unique_ptr<VerifyGrpcConPool> m_pool;
};
