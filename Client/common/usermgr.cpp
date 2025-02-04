#include "usermgr.h"
#include <QJsonArray>
#include "global.h"

UserMgr::~UserMgr()
{

}

void UserMgr::SetUserInfo(std::shared_ptr<UserInfo> user_info) {
    m_user_info = user_info;
}

void UserMgr::SetToken(QString token)
{
    m_token = token;
}

int UserMgr::GetUid()
{
    return m_user_info->m_uid;
}

QString UserMgr::GetName()
{
    return m_user_info->m_name;
}

QString UserMgr::GetIcon()
{
    return m_user_info->m_icon;
}

std::shared_ptr<UserInfo> UserMgr::GetUserInfo()
{
    return m_user_info;
}

void UserMgr::AppendApplyList(QJsonArray array)
{
    // 遍历 QJsonArray 并输出每个元素
    for (const QJsonValue &value : array) {
        auto name = value["name"].toString();
        auto desc = value["desc"].toString();
        auto icon = value["icon"].toString();
        auto nick = value["nick"].toString();
        auto sex = value["sex"].toInt();
        auto uid = value["uid"].toInt();
        auto status = value["status"].toInt();
        auto info = std::make_shared<ApplyInfo>(uid, name,
                                                desc, icon, nick, sex, status);
        m_apply_list.push_back(info);
    }
}

void UserMgr::AppendFriendList(QJsonArray array) {
    // 遍历 QJsonArray 并输出每个元素
    for (const QJsonValue& value : array) {
        auto name = value["name"].toString();
        auto desc = value["desc"].toString();
        auto icon = value["icon"].toString();
        auto nick = value["nick"].toString();
        auto sex = value["sex"].toInt();
        auto uid = value["uid"].toInt();
        auto back = value["back"].toString();

        auto info = std::make_shared<FriendInfo>(uid, name,
                                                 nick, icon, sex, desc, back);
        m_friend_list.push_back(info);
        m_friend_map.insert(uid, info);
    }
}

std::vector<std::shared_ptr<ApplyInfo> > UserMgr::GetApplyList()
{
    return m_apply_list;
}

void UserMgr::AddApplyList(std::shared_ptr<ApplyInfo> app)
{
    m_apply_list.push_back(app);
}

bool UserMgr::AlreadyApply(int uid)
{
    for(auto& apply: m_apply_list){
        if(apply->m_uid == uid){
            return true;
        }
    }

    return false;
}

std::vector<std::shared_ptr<FriendInfo>> UserMgr::GetChatListPerPage() {

    std::vector<std::shared_ptr<FriendInfo>> friend_list;
    int begin = m_chat_loaded;
    int end = begin + CHAT_COUNT_PER_PAGE;

    if (begin >= m_friend_list.size()) {
        return friend_list;
    }

    if (end > m_friend_list.size()) {
        friend_list = std::vector<std::shared_ptr<FriendInfo>>(m_friend_list.begin() + begin, m_friend_list.end());
        return friend_list;
    }


    friend_list = std::vector<std::shared_ptr<FriendInfo>>(m_friend_list.begin() + begin, m_friend_list.begin()+ end);
    return friend_list;
}


std::vector<std::shared_ptr<FriendInfo>> UserMgr::GetConListPerPage() {
    std::vector<std::shared_ptr<FriendInfo>> friend_list;
    int begin = m_contact_loaded;
    int end = begin + CHAT_COUNT_PER_PAGE;

    if (begin >= m_friend_list.size()) {
        return friend_list;
    }

    if (end > m_friend_list.size()) {
        friend_list = std::vector<std::shared_ptr<FriendInfo>>(m_friend_list.begin() + begin, m_friend_list.end());
        return friend_list;
    }


    friend_list = std::vector<std::shared_ptr<FriendInfo>>(m_friend_list.begin() + begin, m_friend_list.begin() + end);
    return friend_list;
}


UserMgr::UserMgr():m_user_info(nullptr), m_chat_loaded(0), m_contact_loaded(0)
{

}

void UserMgr::SlotAddFriendRsp(std::shared_ptr<AuthRsp> rsp)
{
    AddFriend(rsp);
}

void UserMgr::SlotAddFriendAuth(std::shared_ptr<AuthInfo> auth)
{
    AddFriend(auth);
}

bool UserMgr::IsLoadChatFin() {
    if (m_chat_loaded >= m_friend_list.size()) {
        return true;
    }

    return false;
}

void UserMgr::UpdateChatLoadedCount() {
    int begin = m_chat_loaded;
    int end = begin + CHAT_COUNT_PER_PAGE;

    if (begin >= m_friend_list.size()) {
        return ;
    }

    if (end > m_friend_list.size()) {
        m_chat_loaded = m_friend_list.size();
        return ;
    }

    m_chat_loaded = end;
}

void UserMgr::UpdateContactLoadedCount() {
    int begin = m_contact_loaded;
    int end = begin + CHAT_COUNT_PER_PAGE;

    if (begin >= m_friend_list.size()) {
        return;
    }

    if (end > m_friend_list.size()) {
        m_contact_loaded = m_friend_list.size();
        return;
    }

    m_contact_loaded = end;
}

bool UserMgr::IsLoadConFin()
{
    if (m_contact_loaded >= m_friend_list.size()) {
        return true;
    }

    return false;
}

bool UserMgr::CheckFriendById(int uid)
{
    auto iter = m_friend_map.find(uid);
    if(iter == m_friend_map.end()){
        return false;
    }

    return true;
}

void UserMgr::AddFriend(std::shared_ptr<AuthRsp> auth_rsp)
{
    auto friend_info = std::make_shared<FriendInfo>(auth_rsp);
    m_friend_map[friend_info->m_uid] = friend_info;
}

void UserMgr::AddFriend(std::shared_ptr<AuthInfo> auth_info)
{
    auto friend_info = std::make_shared<FriendInfo>(auth_info);
    m_friend_map[friend_info->m_uid] = friend_info;
}

std::shared_ptr<FriendInfo> UserMgr::GetFriendById(int uid)
{
    auto find_it = m_friend_map.find(uid);
    if(find_it == m_friend_map.end()){
        return nullptr;
    }

    return *find_it;
}

void UserMgr::AppendFriendChatMsg(int friend_id,std::vector<std::shared_ptr<TextChatData> > msgs)
{
    auto find_iter = m_friend_map.find(friend_id);
    if(find_iter == m_friend_map.end()){
        qDebug()<<"append friend uid  " << friend_id << " not found";
        return;
    }

    find_iter.value()->AppendChatMsgs(msgs);
}
