#pragma once
#include "Singleton.h"
#include <queue>
#include <thread>
#include <queue>
#include <map>
#include <functional>
#include <unordered_map>
#include "CSession.h"
#include "Const.h"
#include "Data.h"

using FunCallBack = std::function<void(std::shared_ptr<CSession>, const short &msg_id, const std::string &msg_data)>;

class LogicSystem : public Singleton<LogicSystem>
{
public:
	~LogicSystem();
	void PostMsgToQue(std::shared_ptr<LogicNode> msg);

private:
	friend class Singleton<LogicSystem>;
	LogicSystem();
	void DealMsg();
	void RegisterCallBacks();
	void LoginHandler(std::shared_ptr<CSession> session, const short &msg_id, const std::string &msg_data);
	void SearchInfo(std::shared_ptr<CSession> session, const short &msg_id, const std::string &msg_data);
	void AddFriendApply(std::shared_ptr<CSession> session, const short &msg_id, const std::string &msg_data);
	void AuthFriendApply(std::shared_ptr<CSession> session, const short &msg_id, const std::string &msg_data);
	void DealChatTextMsg(std::shared_ptr<CSession> session, const short &msg_id, const std::string &msg_data);
	bool isPureDigit(const std::string &str);
	void GetUserByUid(std::string uid_str, Json::Value &rtvalue);
	void GetUserByName(std::string name, Json::Value &rtvalue);
	bool GetBaseInfo(std::string base_key, int uid, std::shared_ptr<UserInfo> &userinfo);
	bool GetFriendApplyInfo(int to_uid, std::vector<std::shared_ptr<ApplyInfo>> &list);
	bool GetFriendList(int self_id, std::vector<std::shared_ptr<UserInfo>> &user_list);

	std::thread m_worker_thread;
	std::queue<std::shared_ptr<LogicNode>> m_msg_que;
	std::mutex m_mutex;
	std::condition_variable m_consume;
	bool m_stop;
	std::map<short, FunCallBack> m_fun_callbacks;
};