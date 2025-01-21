#include "RedisMgr.h"
#include "Const.h"
#include "ConfigMgr.h"

RedisConPool::RedisConPool(size_t poolSize, const char *host, int port, const char *pwd)
	: m_poolSize(poolSize), m_host(host), m_port(port), m_pwd(pwd), m_stop(false), m_counter(0)
{
	// 创建与链接池同等数量的链接
	for (size_t i = 0; i < m_poolSize; ++i)
	{
		// 通过hiredis的接口创建连接，并检查是否创建成功
		auto *context = redisConnect(host, port);
		if (context == nullptr || context->err != 0)
		{
			if (context != nullptr)
			{
				// 如果连接不为空，释放资源
				redisFree(context);
			}
			continue;
		}

		// 对于每一个成功的连接，发送一条 AUTH 命令给 Redis 进行身份验证
		auto reply = (redisReply *)redisCommand(context, "AUTH %s", pwd);
		if (reply->type == REDIS_REPLY_ERROR)
		{
			std::cout << "认证失败" << std::endl;
			// 释放redisCommand执行后返回的redisReply所占用的内存
			freeReplyObject(reply);
			continue;
		}

		// 释放redisCommand执行后返回的redisReply所占用的内存
		freeReplyObject(reply);
		std::cout << "认证成功" << std::endl;
		m_connections.push(context);
	}

	// 创建一个后台线程 m_check_thread 来周期性地检查连接的状态
	m_check_thread = std::thread([this]()
								 {
			while (!m_stop) {
				m_counter++;
				if (m_counter >= 60) {
					checkThread();
					m_counter = 0;
				}
				std::this_thread::sleep_for(std::chrono::seconds(1));
			} });
}

RedisConPool::~RedisConPool()
{
}

redisContext *RedisConPool::getConnection()
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
	auto *context = m_connections.front();
	m_connections.pop();
	return context;
}

void RedisConPool::returnConnection(redisContext *context)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	if (m_stop)
	{
		return;
	}
	m_connections.push(context);
	m_cond.notify_one();
}

void RedisConPool::ClearConnections()
{
	std::lock_guard<std::mutex> lock(m_mutex);
	while (!m_connections.empty())
	{
		auto *context = m_connections.front();
		redisFree(context);
		m_connections.pop();
	}
}

void RedisConPool::Close()
{
	m_stop = true;
	m_cond.notify_all();
	m_check_thread.join();
}

void RedisConPool::checkThread()
{
	std::lock_guard<std::mutex> lock(m_mutex);
	if (m_stop)
	{
		return;
	}

	// 使用PING命令检查未使用的连接的状态
	auto pool_size = m_connections.size();
	for (int i = 0; i < pool_size && !m_stop; i++)
	{
		auto *context = m_connections.front();
		m_connections.pop();
		try
		{
			// 非异常情况，直接放回队列
			auto reply = (redisReply *)redisCommand(context, "PING");
			if (!reply)
			{
				std::cout << "redis ping failed" << std::endl;
				m_connections.push(context);
				continue;
			}
			freeReplyObject(reply); // 如果成功，释放占用的内存
			m_connections.push(context);
		}
		catch (std::exception &exp)
		{
			// 异常情况，释放连接并重新创建
			std::cout << "Error keeping connection alive: " << exp.what() << std::endl;
			redisFree(context);
			context = redisConnect(m_host, m_port);
			if (context == nullptr || context->err != 0)
			{
				if (context != nullptr)
				{
					redisFree(context);
				}
				continue;
			}

			auto reply = (redisReply *)redisCommand(context, "AUTH %s", m_pwd);
			if (reply->type == REDIS_REPLY_ERROR)
			{
				std::cout << "认证失败" << std::endl;
				freeReplyObject(reply);
				continue;
			}

			freeReplyObject(reply);
			std::cout << "认证成功" << std::endl;
			m_connections.push(context);
		}
	}
}

RedisMgr::RedisMgr()
{
	auto &gCfgMgr = ConfigMgr::Inst();
	auto host = gCfgMgr["Redis"]["Host"];
	auto port = gCfgMgr["Redis"]["Port"];
	auto pwd = gCfgMgr["Redis"]["Passwd"];
	m_pool.reset(new RedisConPool(5, host.c_str(), atoi(port.c_str()), pwd.c_str()));
}

RedisMgr::~RedisMgr()
{
}

// 从 Redis 数据库获取指定键对应的值
bool RedisMgr::Get(const std::string &key, std::string &value)
{
	auto connect = m_pool->getConnection();
	if (connect == nullptr)
	{
		return false;
	}

	auto reply = (redisReply *)redisCommand(connect, "GET %s", key.c_str());
	if (reply == nullptr)
	{
		std::cout << "[ GET  " << key << " ] failed" << std::endl;
		m_pool->returnConnection(connect);
		return false;
	}
	if (reply->type != REDIS_REPLY_STRING)
	{
		std::cout << "[ GET  " << key << " ] failed" << std::endl;
		freeReplyObject(reply);
		m_pool->returnConnection(connect);
		return false;
	}

	value = reply->str;
	freeReplyObject(reply);

	std::cout << "Succeed to execute command [ GET " << key << "  ]" << std::endl;
	m_pool->returnConnection(connect);
	return true;
}

// 将键值对设置到 Redis 数据库中
bool RedisMgr::Set(const std::string &key, const std::string &value)
{
	auto connect = m_pool->getConnection();
	if (connect == nullptr)
	{
		return false;
	}

	auto reply = (redisReply *)redisCommand(connect, "SET %s %s", key.c_str(), value.c_str());
	if (reply == nullptr)
	{
		std::cout << "Execut command [ SET " << key << "  " << value << " ] failure ! " << std::endl;
		m_pool->returnConnection(connect);
		return false;
	}
	if (!(reply->type == REDIS_REPLY_STATUS && (strcmp(reply->str, "OK") == 0 || strcmp(reply->str, "ok") == 0)))
	{
		std::cout << "Execut command [ SET " << key << "  " << value << " ] failure ! " << std::endl;
		freeReplyObject(reply);
		m_pool->returnConnection(connect);
		return false;
	}

	freeReplyObject(reply);
	std::cout << "Execut command [ SET " << key << "  " << value << " ] success ! " << std::endl;
	m_pool->returnConnection(connect);
	return true;
}

// 将一个值推入到 Redis 数据库中指定键对应的列表的左边
bool RedisMgr::LPush(const std::string &key, const std::string &value)
{
	auto connect = m_pool->getConnection();
	if (connect == nullptr)
	{
		return false;
	}

	auto reply = (redisReply *)redisCommand(connect, "LPUSH %s %s", key.c_str(), value.c_str());
	if (reply == nullptr)
	{
		std::cout << "Execut command [ LPUSH " << key << "  " << value << " ] failure ! " << std::endl;
		m_pool->returnConnection(connect);
		return false;
	}

	if (reply->type != REDIS_REPLY_INTEGER || reply->integer <= 0)
	{
		std::cout << "Execut command [ LPUSH " << key << "  " << value << " ] failure ! " << std::endl;
		freeReplyObject(reply);
		m_pool->returnConnection(connect);
		return false;
	}

	std::cout << "Execut command [ LPUSH " << key << "  " << value << " ] success ! " << std::endl;
	freeReplyObject(reply);
	m_pool->returnConnection(connect);
	return true;
}

// 从 Redis 数据库中指定键对应的列表的左边弹出一个元素
bool RedisMgr::LPop(const std::string &key, std::string &value)
{
	auto connect = m_pool->getConnection();
	if (connect == nullptr)
	{
		return false;
	}

	auto reply = (redisReply *)redisCommand(connect, "LPOP %s", key.c_str());
	if (reply == nullptr)
	{
		std::cout << "Execut command [ LPOP " << key << " ] failure ! " << std::endl;
		m_pool->returnConnection(connect);
		return false;
	}
	if (reply->type == REDIS_REPLY_NIL)
	{
		std::cout << "Execut command [ LPOP " << key << " ] failure ! " << std::endl;
		freeReplyObject(reply);
		m_pool->returnConnection(connect);
		return false;
	}

	value = reply->str;
	std::cout << "Execut command [ LPOP " << key << " ] success ! " << std::endl;
	freeReplyObject(reply);
	m_pool->returnConnection(connect);
	return true;
}

// 将一个值推入到 Redis 数据库中指定键对应的列表的右边
bool RedisMgr::RPush(const std::string &key, const std::string &value)
{
	auto connect = m_pool->getConnection();
	if (connect == nullptr)
	{
		return false;
	}

	auto reply = (redisReply *)redisCommand(connect, "RPUSH %s %s", key.c_str(), value.c_str());
	if (reply == nullptr)
	{
		std::cout << "Execut command [ RPUSH " << key << "  " << value << " ] failure ! " << std::endl;
		m_pool->returnConnection(connect);
		return false;
	}

	if (reply->type != REDIS_REPLY_INTEGER || reply->integer <= 0)
	{
		std::cout << "Execut command [ RPUSH " << key << "  " << value << " ] failure ! " << std::endl;
		freeReplyObject(reply);
		m_pool->returnConnection(connect);
		return false;
	}

	std::cout << "Execut command [ RPUSH " << key << "  " << value << " ] success ! " << std::endl;
	freeReplyObject(reply);
	m_pool->returnConnection(connect);
	return true;
}

// 从 Redis 数据库中指定键对应的列表的右边弹出一个元素
bool RedisMgr::RPop(const std::string &key, std::string &value)
{
	auto connect = m_pool->getConnection();
	if (connect == nullptr)
	{
		return false;
	}

	auto reply = (redisReply *)redisCommand(connect, "RPOP %s", key.c_str());
	if (reply == nullptr)
	{
		std::cout << "Execut command [ RPOP " << key << " ] failure ! " << std::endl;
		return false;
	}

	if (reply->type == REDIS_REPLY_NIL)
	{
		std::cout << "Execut command [ RPOP " << key << " ] failure ! " << std::endl;
		freeReplyObject(reply);
		m_pool->returnConnection(connect);
		return false;
	}
	value = reply->str;
	std::cout << "Execut command [ RPOP " << key << " ] success ! " << std::endl;
	freeReplyObject(reply);
	m_pool->returnConnection(connect);
	return true;
}

// 将一个值关联到 Redis 数据库哈希（Hash）中的特定字段
bool RedisMgr::HSet(const std::string &key, const std::string &hkey, const std::string &value)
{
	auto connect = m_pool->getConnection();
	if (connect == nullptr)
	{
		return false;
	}
	auto reply = (redisReply *)redisCommand(connect, "HSET %s %s %s", key.c_str(), hkey.c_str(), value.c_str());
	if (reply == nullptr)
	{
		std::cout << "Execut command [ HSet " << key << "  " << hkey << "  " << value << " ] failure ! " << std::endl;
		m_pool->returnConnection(connect);
		return false;
	}

	if (reply->type != REDIS_REPLY_INTEGER)
	{
		std::cout << "Execut command [ HSet " << key << "  " << hkey << "  " << value << " ] failure ! " << std::endl;
		freeReplyObject(reply);
		m_pool->returnConnection(connect);
		return false;
	}

	std::cout << "Execut command [ HSet " << key << "  " << hkey << "  " << value << " ] success ! " << std::endl;
	freeReplyObject(reply);
	m_pool->returnConnection(connect);
	return true;
}

// 将一个值关联到 Redis 数据库哈希（Hash）中的特定字段
bool RedisMgr::HSet(const char *key, const char *hkey, const char *hvalue, size_t hvaluelen)
{
	auto connect = m_pool->getConnection();
	if (connect == nullptr)
	{
		return false;
	}
	const char *argv[4];
	size_t argvlen[4];
	argv[0] = "HSET";
	argvlen[0] = 4;
	argv[1] = key;
	argvlen[1] = strlen(key);
	argv[2] = hkey;
	argvlen[2] = strlen(hkey);
	argv[3] = hvalue;
	argvlen[3] = hvaluelen;

	auto reply = (redisReply *)redisCommandArgv(connect, 4, argv, argvlen);
	if (reply == nullptr)
	{
		std::cout << "Execut command [ HSet " << key << "  " << hkey << "  " << hvalue << " ] failure ! " << std::endl;
		m_pool->returnConnection(connect);
		return false;
	}

	if (reply->type != REDIS_REPLY_INTEGER)
	{
		std::cout << "Execut command [ HSet " << key << "  " << hkey << "  " << hvalue << " ] failure ! " << std::endl;
		freeReplyObject(reply);
		m_pool->returnConnection(connect);
		return false;
	}
	std::cout << "Execut command [ HSet " << key << "  " << hkey << "  " << hvalue << " ] success ! " << std::endl;
	freeReplyObject(reply);
	m_pool->returnConnection(connect);
	return true;
}

// 从 Redis 数据库哈希（Hash）中获取特定字段的值
std::string RedisMgr::HGet(const std::string &key, const std::string &hkey)
{
	auto connect = m_pool->getConnection();
	if (connect == nullptr)
	{
		return "";
	}
	const char *argv[3];
	size_t argvlen[3];
	argv[0] = "HGET";
	argvlen[0] = 4;
	argv[1] = key.c_str();
	argvlen[1] = key.length();
	argv[2] = hkey.c_str();
	argvlen[2] = hkey.length();

	auto reply = (redisReply *)redisCommandArgv(connect, 3, argv, argvlen);
	if (reply == nullptr)
	{
		std::cout << "Execut command [ HGet " << key << " " << hkey << "  ] failure ! " << std::endl;
		m_pool->returnConnection(connect);
		return "";
	}

	if (reply->type == REDIS_REPLY_NIL)
	{
		freeReplyObject(reply);
		std::cout << "Execut command [ HGet " << key << " " << hkey << "  ] failure ! " << std::endl;
		m_pool->returnConnection(connect);
		return "";
	}

	std::string value = reply->str;
	freeReplyObject(reply);
	m_pool->returnConnection(connect);
	std::cout << "Execut command [ HGet " << key << " " << hkey << " ] success ! " << std::endl;
	return value;
}

// 从 Redis 数据库哈希（Hash）中删除特定字段
bool RedisMgr::HDel(const std::string &key, const std::string &field)
{
	auto connect = m_pool->getConnection();
	if (connect == nullptr)
	{
		return false;
	}

	Defer defer([&connect, this]()
				{ m_pool->returnConnection(connect); });

	redisReply *reply = (redisReply *)redisCommand(connect, "HDEL %s %s", key.c_str(), field.c_str());
	if (reply == nullptr)
	{
		std::cout << "HDEL command failed" << std::endl;
		return false;
	}

	bool success = false;
	if (reply->type == REDIS_REPLY_INTEGER)
	{
		success = reply->integer > 0;
	}

	freeReplyObject(reply);
	return success;
}

// 从 Redis 数据库中删除键
bool RedisMgr::Del(const std::string &key)
{
	auto connect = m_pool->getConnection();
	if (connect == nullptr)
	{
		return false;
	}
	auto reply = (redisReply *)redisCommand(connect, "DEL %s", key.c_str());
	if (reply == nullptr)
	{
		std::cout << "Execut command [ Del " << key << " ] failure ! " << std::endl;
		m_pool->returnConnection(connect);
		return false;
	}

	if (reply->type != REDIS_REPLY_INTEGER)
	{
		std::cout << "Execut command [ Del " << key << " ] failure ! " << std::endl;
		freeReplyObject(reply);
		m_pool->returnConnection(connect);
		return false;
	}

	std::cout << "Execut command [ Del " << key << " ] success ! " << std::endl;
	freeReplyObject(reply);
	m_pool->returnConnection(connect);
	return true;
}

// 检查 Redis 数据库中是否存在名为 key 的键
bool RedisMgr::ExistsKey(const std::string &key)
{
	auto connect = m_pool->getConnection();
	if (connect == nullptr)
	{
		return false;
	}

	auto reply = (redisReply *)redisCommand(connect, "exists %s", key.c_str());
	if (reply == nullptr)
	{
		std::cout << "Not Found [ Key " << key << " ]  ! " << std::endl;
		m_pool->returnConnection(connect);
		return false;
	}

	if (reply->type != REDIS_REPLY_INTEGER || reply->integer == 0)
	{
		std::cout << "Not Found [ Key " << key << " ]  ! " << std::endl;
		m_pool->returnConnection(connect);
		freeReplyObject(reply);
		return false;
	}
	std::cout << " Found [ Key " << key << " ] exists ! " << std::endl;
	freeReplyObject(reply);
	m_pool->returnConnection(connect);
	return true;
}

void RedisMgr::Close()
{
	m_pool->Close();
	m_pool->ClearConnections();
}
