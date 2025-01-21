#pragma once
#include "Singleton.h"
#include "MysqlDao.h"

/* MySQL数据库管理类，用于管理数据库操作 */
class MysqlMgr : public Singleton<MysqlMgr>
{
public:
	~MysqlMgr();

	bool AddFriendApply(const int &from, const int &to);
	bool AuthFriendApply(const int &from, const int &to);
	bool AddFriend(const int &from, const int &to, std::string back_name);
	bool GetApplyList(int touid, std::vector<std::shared_ptr<ApplyInfo>> &applyList, int begin, int limit = 10);
	bool GetFriendList(int self_id, std::vector<std::shared_ptr<UserInfo>> &user_info);
	std::shared_ptr<UserInfo> GetUser(int uid);
	std::shared_ptr<UserInfo> GetUser(std::string name);

private:
	friend class Singleton<MysqlMgr>;
	MysqlMgr();

	MysqlDao m_dao;
};