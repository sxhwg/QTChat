#include "MysqlMgr.h"

MysqlMgr::~MysqlMgr()
{
}

int MysqlMgr::RegUser(const std::string &name, const std::string &email, const std::string &pwd, const std::string &icon)
{
	return m_dao.RegUser(name, email, pwd, icon);
}

bool MysqlMgr::CheckEmail(const std::string &name, const std::string &email)
{
	return m_dao.CheckEmail(name, email);
}

bool MysqlMgr::UpdatePwd(const std::string &name, const std::string &pwd)
{
	return m_dao.UpdatePwd(name, pwd);
}

bool MysqlMgr::CheckPwd(const std::string &email, const std::string &pwd, UserInfo &userInfo)
{
	return m_dao.CheckPwd(email, pwd, userInfo);
}

MysqlMgr::MysqlMgr()
{
}
