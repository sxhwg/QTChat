#include "UserMgr.h"
#include "CSession.h"
#include "RedisMgr.h"

UserMgr::~UserMgr()
{
    m_uid_to_session.clear();
}

std::shared_ptr<CSession> UserMgr::GetSession(int uid)
{
    std::lock_guard<std::mutex> lock(m_session_mtx);
    auto iter = m_uid_to_session.find(uid);
    if (iter == m_uid_to_session.end())
    {
        return nullptr;
    }

    return iter->second;
}

void UserMgr::SetUserSession(int uid, std::shared_ptr<CSession> session)
{
    std::lock_guard<std::mutex> lock(m_session_mtx);
    m_uid_to_session[uid] = session;
}

void UserMgr::RmvUserSession(int uid)
{
    auto uid_str = std::to_string(uid);

    {
        std::lock_guard<std::mutex> lock(m_session_mtx);
        m_uid_to_session.erase(uid);
    }
}

UserMgr::UserMgr()
{
}
