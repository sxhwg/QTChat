#include "MysqlMgr.h"

MysqlMgr::~MysqlMgr()
{
}

bool MysqlMgr::AddFriendApply(const int &from, const int &to)
{
	return m_dao.AddFriendApply(from, to);
}

bool MysqlMgr::AuthFriendApply(const int &from, const int &to)
{
	return m_dao.AuthFriendApply(from, to);
}

bool MysqlMgr::AddFriend(const int &from, const int &to, std::string back_name)
{
	return m_dao.AddFriend(from, to, back_name);
}

bool MysqlMgr::GetApplyList(int touid, std::vector<std::shared_ptr<ApplyInfo>> &applyList, int begin, int limit)
{
	return m_dao.GetApplyList(touid, applyList, begin, limit);
}

bool MysqlMgr::GetFriendList(int self_id, std::vector<std::shared_ptr<UserInfo>> &user_info)
{
	return m_dao.GetFriendList(self_id, user_info);
}

std::shared_ptr<UserInfo> MysqlMgr::GetUser(int uid)
{
	return m_dao.GetUser(uid);
}

std::shared_ptr<UserInfo> MysqlMgr::GetUser(std::string name)
{
	return m_dao.GetUser(name);
}

MysqlMgr::MysqlMgr()
{
}
