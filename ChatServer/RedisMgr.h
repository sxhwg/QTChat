#pragma once
#include "Const.h"
#include <hiredis/hiredis.h>
#include <queue>
#include <atomic>
#include <mutex>
#include <thread>
#include <memory>
#include "Singleton.h"

/* Redis链接池，用于提高并发性能 */
class RedisConPool
{
public:
	RedisConPool(size_t poolSize, const char *host, int port, const char *pwd);
	~RedisConPool();

	redisContext *getConnection();
	void returnConnection(redisContext *context);
	void ClearConnections();
	void Close();

private:
	void checkThread();

	std::atomic<bool> m_stop;
	size_t m_poolSize;
	const char *m_host;
	const char *m_pwd;
	int m_port;
	std::queue<redisContext *> m_connections;
	std::mutex m_mutex;
	std::condition_variable m_cond;
	std::thread m_check_thread;
	int m_counter;
};

/* Redis数据库管理类，用于管理和处理数据库操作 */
class RedisMgr : public Singleton<RedisMgr>,
				 public std::enable_shared_from_this<RedisMgr>
{
public:
	~RedisMgr();
	bool Get(const std::string &key, std::string &value);
	bool Set(const std::string &key, const std::string &value);
	bool LPush(const std::string &key, const std::string &value);
	bool LPop(const std::string &key, std::string &value);
	bool RPush(const std::string &key, const std::string &value);
	bool RPop(const std::string &key, std::string &value);
	bool HSet(const std::string &key, const std::string &hkey, const std::string &value);
	bool HSet(const char *key, const char *hkey, const char *hvalue, size_t hvaluelen);
	std::string HGet(const std::string &key, const std::string &hkey);
	bool HDel(const std::string &key, const std::string &field);
	bool Del(const std::string &key);
	bool ExistsKey(const std::string &key);
	void Close();

private:
	friend class Singleton<RedisMgr>;
	RedisMgr();
	std::unique_ptr<RedisConPool> m_pool;
};