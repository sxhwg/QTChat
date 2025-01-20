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

bool MysqlDao::AddFriendApply(const int &from, const int &to)
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
		std::unique_ptr<sql::PreparedStatement> pstmt(con->m_con->prepareStatement("INSERT INTO friend_apply (from_uid, to_uid) values (?,?) "
																				  "ON DUPLICATE KEY UPDATE from_uid = from_uid, to_uid = to_uid"));
		pstmt->setInt(1, from); // from id
		pstmt->setInt(2, to);
		// 执行更新
		int rowAffected = pstmt->executeUpdate();
		if (rowAffected < 0)
		{
			return false;
		}
		return true;
	}
	catch (sql::SQLException &e)
	{
		std::cerr << "SQLException: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		return false;
	}

	return true;
}

bool MysqlDao::AuthFriendApply(const int &from, const int &to)
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
		std::unique_ptr<sql::PreparedStatement> pstmt(con->m_con->prepareStatement("UPDATE friend_apply SET status = 1 "
																				   "WHERE from_uid = ? AND to_uid = ?"));
		// 反过来的申请时from，验证时to
		pstmt->setInt(1, to); // from id
		pstmt->setInt(2, from);
		// 执行更新
		int rowAffected = pstmt->executeUpdate();
		if (rowAffected < 0)
		{
			return false;
		}
		return true;
	}
	catch (sql::SQLException &e)
	{
		std::cerr << "SQLException: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		return false;
	}

	return true;
}

bool MysqlDao::AddFriend(const int &from, const int &to, std::string back_name)
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

		// 开始事务
		con->m_con->setAutoCommit(false);

		// 准备第一个SQL语句, 插入认证方好友数据
		std::unique_ptr<sql::PreparedStatement> pstmt(con->m_con->prepareStatement("INSERT IGNORE INTO friend(self_id, friend_id, back) "
																				   "VALUES (?, ?, ?) "));
		// 反过来的申请时from，验证时to
		pstmt->setInt(1, from); // from id
		pstmt->setInt(2, to);
		pstmt->setString(3, back_name);
		// 执行更新
		int rowAffected = pstmt->executeUpdate();
		if (rowAffected < 0)
		{
			con->m_con->rollback();
			return false;
		}

		// 准备第二个SQL语句，插入申请方好友数据
		std::unique_ptr<sql::PreparedStatement> pstmt2(con->m_con->prepareStatement("INSERT IGNORE INTO friend(self_id, friend_id, back) "
																					"VALUES (?, ?, ?) "));
		// 反过来的申请时from，验证时to
		pstmt2->setInt(1, to); // from id
		pstmt2->setInt(2, from);
		pstmt2->setString(3, "");
		// 执行更新
		int rowAffected2 = pstmt2->executeUpdate();
		if (rowAffected2 < 0)
		{
			con->m_con->rollback();
			return false;
		}

		// 提交事务
		con->m_con->commit();
		std::cout << "addfriend insert friends success" << std::endl;

		return true;
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
		return false;
	}

	return true;
}

bool MysqlDao::GetApplyList(int touid, std::vector<std::shared_ptr<ApplyInfo>> &applyList, int begin, int limit)
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
		// 准备SQL语句, 根据起始id和限制条数返回列表
		std::unique_ptr<sql::PreparedStatement> pstmt(con->m_con->prepareStatement("select apply.from_uid, apply.status, user.name, "
																				   "user.nick, user.sex from friend_apply as apply join user on apply.from_uid = user.uid where apply.to_uid = ? "
																				   "and apply.id > ? order by apply.id ASC LIMIT ? "));

		pstmt->setInt(1, touid); // 将uid替换为你要查询的uid
		pstmt->setInt(2, begin); // 起始id
		pstmt->setInt(3, limit); // 偏移量
		// 执行查询
		std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
		// 遍历结果集
		while (res->next())
		{
			auto name = res->getString("name");
			auto uid = res->getInt("from_uid");
			auto status = res->getInt("status");
			auto nick = res->getString("nick");
			auto sex = res->getInt("sex");
			auto apply_ptr = std::make_shared<ApplyInfo>(uid, name, "", "", nick, sex, status);
			applyList.push_back(apply_ptr);
		}
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

bool MysqlDao::GetFriendList(int self_id, std::vector<std::shared_ptr<UserInfo>> &user_info_list)
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
		// 准备SQL语句, 根据起始id和限制条数返回列表
		std::unique_ptr<sql::PreparedStatement> pstmt(con->m_con->prepareStatement("select * from friend where self_id = ? "));

		pstmt->setInt(1, self_id); // 将uid替换为你要查询的uid

		// 执行查询
		std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
		// 遍历结果集
		while (res->next())
		{
			auto friend_id = res->getInt("friend_id");
			auto back = res->getString("back");
			// 再一次查询friend_id对应的信息
			auto user_info = GetUser(friend_id);
			if (user_info == nullptr)
			{
				continue;
			}

			user_info->back = user_info->name;
			user_info_list.push_back(user_info);
		}
		return true;
	}
	catch (sql::SQLException &e)
	{
		std::cerr << "SQLException: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		return false;
	}

	return true;
}

std::shared_ptr<UserInfo> MysqlDao::GetUser(int uid)
{
	auto con = m_pool->getConnection();
	if (con == nullptr)
	{
		return nullptr;
	}

	Defer defer([this, &con]()
				{ m_pool->returnConnection(std::move(con)); });

	try
	{
		// 准备SQL语句
		std::unique_ptr<sql::PreparedStatement> pstmt(con->m_con->prepareStatement("SELECT * FROM user WHERE uid = ?"));
		pstmt->setInt(1, uid); // 将uid替换为你要查询的uid

		// 执行查询
		std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
		std::shared_ptr<UserInfo> user_ptr = nullptr;
		// 遍历结果集
		while (res->next())
		{
			user_ptr.reset(new UserInfo);
			user_ptr->pwd = res->getString("pwd");
			user_ptr->email = res->getString("email");
			user_ptr->name = res->getString("name");
			user_ptr->nick = res->getString("nick");
			user_ptr->desc = res->getString("desc");
			user_ptr->sex = res->getInt("sex");
			user_ptr->icon = res->getString("icon");
			user_ptr->uid = uid;
			break;
		}
		return user_ptr;
	}
	catch (sql::SQLException &e)
	{
		std::cerr << "SQLException: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		return nullptr;
	}
}

std::shared_ptr<UserInfo> MysqlDao::GetUser(std::string name)
{
	auto con = m_pool->getConnection();
	if (con == nullptr)
	{
		return nullptr;
	}

	Defer defer([this, &con]()
				{ m_pool->returnConnection(std::move(con)); });

	try
	{
		// 准备SQL语句
		std::unique_ptr<sql::PreparedStatement> pstmt(con->m_con->prepareStatement("SELECT * FROM user WHERE name = ?"));
		pstmt->setString(1, name); // 将uid替换为你要查询的uid

		// 执行查询
		std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
		std::shared_ptr<UserInfo> user_ptr = nullptr;
		// 遍历结果集
		while (res->next())
		{
			user_ptr.reset(new UserInfo);
			user_ptr->pwd = res->getString("pwd");
			user_ptr->email = res->getString("email");
			user_ptr->name = res->getString("name");
			user_ptr->nick = res->getString("nick");
			user_ptr->desc = res->getString("desc");
			user_ptr->sex = res->getInt("sex");
			user_ptr->uid = res->getInt("uid");
			break;
		}
		return user_ptr;
	}
	catch (sql::SQLException &e)
	{
		std::cerr << "SQLException: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		return nullptr;
	}
}
