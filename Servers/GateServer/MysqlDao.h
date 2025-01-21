#pragma once
#include <thread>
#include <queue>
#include <condition_variable>
#include <atomic>
#include <jdbc/mysql_driver.h>
#include <jdbc/mysql_connection.h>
#include <jdbc/cppconn/prepared_statement.h>
#include <jdbc/cppconn/resultset.h>
#include <jdbc/cppconn/statement.h>
#include <jdbc/cppconn/exception.h>

struct UserInfo
{
	std::string name;
	std::string pwd;
	int uid;
	std::string email;
};

class SqlConnection
{
public:
	SqlConnection(sql::Connection *con, int64_t last_oper_time);

	std::unique_ptr<sql::Connection> m_con;
	int64_t m_last_oper_time;
};

class MySQLConPool
{
public:
	MySQLConPool(const std::string &url, const std::string &user, const std::string &pass,
				 const std::string &schema, int poolSize);
	~MySQLConPool();

	std::unique_ptr<SqlConnection> getConnection();
	void returnConnection(std::unique_ptr<SqlConnection> con);
	void Close();

private:
	void checkConnection();

	std::string m_url;
	std::string m_user;
	std::string m_pass;
	std::string m_schema;
	int m_poolSize;
	std::queue<std::unique_ptr<SqlConnection>> m_connections;
	std::mutex m_mutex;
	std::condition_variable m_cond;
	std::atomic<bool> m_stop;
	std::thread m_check_thread;
};

/* MySQL数据访问类，用于处理数据库操作 */
class MysqlDao
{
public:
	MysqlDao();
	~MysqlDao();

	int RegUser(const std::string &name, const std::string &email, const std::string &pwd, const std::string &icon);
	bool CheckEmail(const std::string &name, const std::string &email);
	bool UpdatePwd(const std::string &name, const std::string &newpwd);
	bool CheckPwd(const std::string &name, const std::string &pwd, UserInfo &userInfo);

private:
	std::unique_ptr<MySQLConPool> m_pool;
};