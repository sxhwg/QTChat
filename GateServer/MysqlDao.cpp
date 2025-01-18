#include "MysqlDao.h"
#include "ConfigMgr.h"

SqlConnection::SqlConnection(sql::Connection *con, int64_t last_oper_time) : m_con(con), m_last_oper_time(last_oper_time)
{
}

MySQLConPool::MySQLConPool(const std::string &url, const std::string &user, const std::string &pass,
						   const std::string &schema, int poolSize)
	: m_url(url), m_user(user), m_pass(pass), m_schema(schema), m_poolSize(poolSize), m_stop(false)
{
	try
	{
		for (int i = 0; i < m_poolSize; ++i)
		{
			sql::mysql::MySQL_Driver *driver = sql::mysql::get_mysql_driver_instance();
			auto *con = driver->connect(m_url, m_user, m_pass);
			con->setSchema(m_schema);
			// 获取当前时间戳
			auto currentTime = std::chrono::system_clock::now().time_since_epoch();
			// 将时间戳转换为秒
			int64_t timestamp = std::chrono::duration_cast<std::chrono::seconds>(currentTime).count();
			m_connections.push(std::make_unique<SqlConnection>(con, timestamp));
		}

		m_check_thread = std::thread([this]()
									 {
				while (!m_stop) {
					checkConnection();
					std::this_thread::sleep_for(std::chrono::seconds(60));
				} });

		m_check_thread.detach();
	}
	catch (sql::SQLException &e)
	{
		// 处理异常
		std::cout << "mysql pool init failed, error is " << e.what() << std::endl;
	}
}

MySQLConPool::~MySQLConPool()
{
	std::unique_lock<std::mutex> lock(m_mutex);
	while (!m_connections.empty())
	{
		m_connections.pop();
	}
}

std::unique_ptr<SqlConnection> MySQLConPool::getConnection()
{
	std::unique_lock<std::mutex> lock(m_mutex);
	m_cond.wait(lock, [this]
				{
					if (m_stop)
					{
						return true;
					}
					return !m_connections.empty(); });
	if (m_stop)
	{
		return nullptr;
	}
	std::unique_ptr<SqlConnection> con(std::move(m_connections.front()));
	m_connections.pop();
	return con;
}

void MySQLConPool::returnConnection(std::unique_ptr<SqlConnection> con)
{
	std::unique_lock<std::mutex> lock(m_mutex);
	if (m_stop)
	{
		return;
	}
	m_connections.push(std::move(con));
	m_cond.notify_one();
}

void MySQLConPool::Close()
{
	m_stop = true;
	m_cond.notify_all();
}

void MySQLConPool::checkConnection()
{
	std::lock_guard<std::mutex> guard(m_mutex);
	int poolsize = m_connections.size();
	// 获取当前时间戳
	auto currentTime = std::chrono::system_clock::now().time_since_epoch();
	// 将时间戳转换为秒
	long long timestamp = std::chrono::duration_cast<std::chrono::seconds>(currentTime).count();
	for (int i = 0; i < poolsize; i++)
	{
		auto con = std::move(m_connections.front());
		m_connections.pop();
		Defer defer([this, &con]()
					{ m_connections.push(std::move(con)); });

		if (timestamp - con->m_last_oper_time < 5)
		{
			continue;
		}

		try
		{
			std::unique_ptr<sql::Statement> stmt(con->m_con->createStatement());
			stmt->executeQuery("SELECT 1");
			con->m_last_oper_time = timestamp;
			// std::cout << "execute timer alive query , cur is " << timestamp << std::endl;
		}
		catch (sql::SQLException &e)
		{
			std::cout << "Error keeping connection alive: " << e.what() << std::endl;
			// 重新创建连接并替换旧的连接
			sql::mysql::MySQL_Driver *driver = sql::mysql::get_mysql_driver_instance();
			auto *newcon = driver->connect(m_url, m_user, m_pass);
			newcon->setSchema(m_schema);
			con->m_con.reset(newcon);
			con->m_last_oper_time = timestamp;
		}
	}
}

MysqlDao::MysqlDao()
{
	auto &cfg = ConfigMgr::Inst();
	const auto &host = cfg["Mysql"]["Host"];
	const auto &port = cfg["Mysql"]["Port"];
	const auto &pwd = cfg["Mysql"]["Passwd"];
	const auto &schema = cfg["Mysql"]["Schema"];
	const auto &user = cfg["Mysql"]["User"];
	m_pool.reset(new MySQLConPool(host + ":" + port, user, pwd, schema, 5));
}

MysqlDao::~MysqlDao()
{
	m_pool->Close();
}

int MysqlDao::RegUser(const std::string &name, const std::string &email, const std::string &pwd,
					  const std::string &icon)
{
	auto con = m_pool->getConnection();
	if (con == nullptr)
	{
		return false;
	}

	Defer defer([this, &con]
				{ m_pool->returnConnection(std::move(con)); });

	try
	{
		// 开始事务
		con->m_con->setAutoCommit(false);

		// 执行第一个数据库操作，根据email查找用户
		// 准备查询语句
		std::unique_ptr<sql::PreparedStatement> pstmt_email(con->m_con->prepareStatement("SELECT 1 FROM user WHERE email = ?"));

		// 绑定参数
		pstmt_email->setString(1, email);

		// 执行查询
		std::unique_ptr<sql::ResultSet> res_email(pstmt_email->executeQuery());

		auto email_exist = res_email->next();
		if (email_exist)
		{
			con->m_con->rollback();
			std::cout << "email " << email << " exist";
			return 0;
		}

		// 准备查询用户名是否重复
		std::unique_ptr<sql::PreparedStatement> pstmt_name(con->m_con->prepareStatement("SELECT 1 FROM user WHERE name = ?"));

		// 绑定参数
		pstmt_name->setString(1, name);

		// 执行查询
		std::unique_ptr<sql::ResultSet> res_name(pstmt_name->executeQuery());

		auto name_exist = res_name->next();
		if (name_exist)
		{
			con->m_con->rollback();
			std::cout << "name " << name << " exist";
			return 0;
		}

		// 准备更新用户id
		std::unique_ptr<sql::PreparedStatement> pstmt_upid(con->m_con->prepareStatement("UPDATE user_id SET id = id + 1"));

		// 执行更新
		pstmt_upid->executeUpdate();

		// 获取更新后的 id 值
		std::unique_ptr<sql::PreparedStatement> pstmt_uid(con->m_con->prepareStatement("SELECT id FROM user_id"));
		std::unique_ptr<sql::ResultSet> res_uid(pstmt_uid->executeQuery());
		int newId = 0;
		// 处理结果集
		if (res_uid->next())
		{
			newId = res_uid->getInt("id");
		}
		else
		{
			std::cout << "select id from user_id failed" << std::endl;
			con->m_con->rollback();
			return -1;
		}

		// 插入user信息
		std::unique_ptr<sql::PreparedStatement> pstmt_insert(con->m_con->prepareStatement("INSERT INTO user (uid, name, email, pwd, nick, icon) "
																						  "VALUES (?, ?, ?, ?,?,?)"));
		pstmt_insert->setInt(1, newId);
		pstmt_insert->setString(2, name);
		pstmt_insert->setString(3, email);
		pstmt_insert->setString(4, pwd);
		pstmt_insert->setString(5, name);
		pstmt_insert->setString(6, icon);
		// 执行插入
		pstmt_insert->executeUpdate();
		// 提交事务
		con->m_con->commit();
		std::cout << "newuser insert into user success" << std::endl;
		return newId;
	}
	catch (sql::SQLException &e)
	{
		// 如果发生错误，回滚事务
		if (con)
		{
			con->m_con->rollback();
		}
		std::cerr << "SQLException: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		return -1;
	}
}

bool MysqlDao::CheckEmail(const std::string &name, const std::string &email)
{
	auto con = m_pool->getConnection();
	try
	{
		if (con == nullptr)
		{
			return false;
		}

		// 准备查询语句
		std::unique_ptr<sql::PreparedStatement> pstmt(con->m_con->prepareStatement("SELECT email FROM user WHERE name = ?"));

		// 绑定参数
		pstmt->setString(1, name);

		// 执行查询
		std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

		// 遍历结果集
		while (res->next())
		{
			std::cout << "Check Email: " << res->getString("email") << std::endl;
			if (email != res->getString("email"))
			{
				m_pool->returnConnection(std::move(con));
				return false;
			}
			m_pool->returnConnection(std::move(con));
			return true;
		}
		return false;
	}
	catch (sql::SQLException &e)
	{
		m_pool->returnConnection(std::move(con));
		std::cerr << "SQLException: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		return false;
	}
}

bool MysqlDao::UpdatePwd(const std::string &name, const std::string &newpwd)
{
	auto con = m_pool->getConnection();
	try
	{
		if (con == nullptr)
		{
			return false;
		}

		// 准备查询语句
		std::unique_ptr<sql::PreparedStatement> pstmt(con->m_con->prepareStatement("UPDATE user SET pwd = ? WHERE name = ?"));

		// 绑定参数
		pstmt->setString(2, name);
		pstmt->setString(1, newpwd);

		// 执行更新
		int updateCount = pstmt->executeUpdate();

		std::cout << "Updated rows: " << updateCount << std::endl;
		m_pool->returnConnection(std::move(con));
		return true;
	}
	catch (sql::SQLException &e)
	{
		m_pool->returnConnection(std::move(con));
		std::cerr << "SQLException: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		return false;
	}
}

bool MysqlDao::CheckPwd(const std::string &email, const std::string &pwd, UserInfo &userInfo)
{
	auto con = m_pool->getConnection();
	if (con == nullptr)
	{
		return false;
	}

	Defer defer([this, &con]()
				{ m_pool->returnConnection(std::move(con)); });

	try
	{

		// 准备SQL语句
		std::unique_ptr<sql::PreparedStatement> pstmt(con->m_con->prepareStatement("SELECT * FROM user WHERE email = ?"));
		pstmt->setString(1, email);

		// 执行查询
		std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
		std::string origin_pwd = "";
		// 遍历结果集
		while (res->next())
		{
			origin_pwd = res->getString("pwd");
			// 输出查询到的密码
			std::cout << "Password: " << origin_pwd << std::endl;
			break;
		}

		if (pwd != origin_pwd)
		{
			return false;
		}
		userInfo.name = res->getString("name");
		userInfo.email = res->getString("email");
		userInfo.uid = res->getInt("uid");
		userInfo.pwd = origin_pwd;
		return true;
	}
	catch (sql::SQLException &e)
	{
		std::cerr << "SQLException: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		return false;
	}
}