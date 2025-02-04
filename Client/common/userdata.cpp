#include "userdata.h"
SearchInfo::SearchInfo(int uid, QString name,QString nick, QString desc, int sex, QString icon)
    :m_uid(uid), m_name(name), m_nick(nick), m_desc(desc), m_sex(sex), m_icon(icon)
{

}

AddFriendApply::AddFriendApply(int from_uid, QString name, QString desc, QString icon, QString nick, int sex)
    :m_from_uid(from_uid), m_name(name), m_desc(desc), m_icon(icon), m_nick(nick), m_sex(sex)
{

}

ApplyInfo::ApplyInfo(int uid, QString name, QString desc, QString icon, QString nick, int sex, int status)
    :m_uid(uid),m_name(name),m_desc(desc),m_icon(icon),m_nick(nick),m_sex(sex),m_status(status)
{

}

ApplyInfo::ApplyInfo(std::shared_ptr<AddFriendApply> addinfo)
    :m_uid(addinfo->m_from_uid),m_name(addinfo->m_name),m_desc(addinfo->m_desc),m_icon(addinfo->m_icon),
    m_nick(addinfo->m_nick),m_sex(addinfo->m_sex),m_status(0)
{

}

void ApplyInfo::SetIcon(QString head)
{
    m_icon = head;
}

AuthInfo::AuthInfo(int uid, QString name, QString nick, QString icon, int sex)
    :m_uid(uid), m_name(name), m_nick(nick), m_icon(icon), m_sex(sex)
{

}

AuthRsp::AuthRsp(int peer_uid, QString peer_name, QString peer_nick, QString peer_icon, int peer_sex)
    :m_uid(peer_uid),m_name(peer_name),m_nick(peer_nick),m_icon(peer_icon),m_sex(peer_sex)
{

}

TextChatData::TextChatData(QString msg_id, QString msg_content, int fromuid, int touid)
    :m_msg_id(msg_id),m_msg_content(msg_content),m_from_uid(fromuid),m_to_uid(touid)
{

}

TextChatMsg::TextChatMsg(int fromuid, int touid, QJsonArray arrays)
    :m_from_uid(fromuid),m_to_uid(touid)
{
    for(auto  msg_data : arrays){
        auto msg_obj = msg_data.toObject();
        auto content = msg_obj["content"].toString();
        auto msgid = msg_obj["msgid"].toString();
        auto msg_ptr = std::make_shared<TextChatData>(msgid, content,fromuid, touid);
        m_chat_msgs.push_back(msg_ptr);
    }
}

FriendInfo::FriendInfo(int uid, QString name, QString nick, QString icon, int sex, QString desc,
                       QString back, QString last_msg)
    :m_uid(uid),m_name(name),m_nick(nick),m_icon(icon),m_sex(sex),m_desc(desc),m_back(back),m_last_msg(last_msg)
{

}

FriendInfo::FriendInfo(std::shared_ptr<AuthInfo> auth_info)
    :m_uid(auth_info->m_uid),m_nick(auth_info->m_nick),m_icon(auth_info->m_icon),
    m_name(auth_info->m_name),m_sex(auth_info->m_sex)
{

}

FriendInfo::FriendInfo(std::shared_ptr<AuthRsp> auth_rsp)
    :m_uid(auth_rsp->m_uid),m_nick(auth_rsp->m_nick),m_icon(auth_rsp->m_icon),
    m_name(auth_rsp->m_name),m_sex(auth_rsp->m_sex)
{

}

void FriendInfo::AppendChatMsgs(const std::vector<std::shared_ptr<TextChatData>> text_vec)
{
    for(const auto & text: text_vec){
        m_chat_msgs.push_back(text);
    }
}

UserInfo::UserInfo(int uid, QString name, QString nick, QString icon, int sex, QString last_msg)
    :m_uid(uid),m_name(name),m_nick(nick),m_icon(icon),m_sex(sex),m_last_msg(last_msg)
{

}

UserInfo::UserInfo(std::shared_ptr<AuthInfo> auth)
    :m_uid(auth->m_uid),m_name(auth->m_name),m_nick(auth->m_nick),
    m_icon(auth->m_icon),m_sex(auth->m_sex),m_last_msg("")
{

}

UserInfo::UserInfo(int uid, QString name, QString icon)
    :m_uid(uid), m_name(name), m_icon(icon),m_nick(m_name),m_sex(0),m_last_msg("")
{

}

UserInfo::UserInfo(std::shared_ptr<AuthRsp> auth)
    :m_uid(auth->m_uid),m_name(auth->m_name),m_nick(auth->m_nick),
    m_icon(auth->m_icon),m_sex(auth->m_sex),m_last_msg("")
{

}

UserInfo::UserInfo(std::shared_ptr<SearchInfo> search_info)
    :m_uid(search_info->m_uid),m_name(search_info->m_name),m_nick(search_info->m_nick),
    m_icon(search_info->m_icon),m_sex(search_info->m_sex),m_last_msg("")
{

}

UserInfo::UserInfo(std::shared_ptr<FriendInfo> friend_info)
    :m_uid(friend_info->m_uid),m_name(friend_info->m_name),m_nick(friend_info->m_nick),
    m_icon(friend_info->m_icon),m_sex(friend_info->m_sex),m_last_msg("")
{
    m_chat_msgs = friend_info->m_chat_msgs;
}
