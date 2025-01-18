#pragma once
#include "Singleton.h"
#include "MysqlDao.h"

/* MySQL数据库管理类，用于管理数据库操作 */
class MysqlMgr : public Singleton<MysqlMgr>
{
public:
	~MysqlMgr();

	int RegUser(const std::string &name, const std::string &email, const std::string &pwd, const std::string &icon);
	bool CheckEmail(const std::string &name, const std::string &email);
	bool UpdatePwd(const std::string &name, const std::string &pwd);
	bool CheckPwd(const std::string &email, const std::string &pwd, UserInfo &userInfo);

private:
	friend class Singleton<MysqlMgr>;
	MysqlMgr();

	MysqlDao m_dao;
};