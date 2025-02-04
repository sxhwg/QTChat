#ifndef USERDATA_H
#define USERDATA_H
#include <QString>
#include <memory>
#include <QJsonArray>
#include <vector>
#include <QJsonObject>

struct SearchInfo {
    SearchInfo(int uid, QString name, QString nick, QString desc, int sex, QString icon);
    int m_uid;
    QString m_name;
    QString m_nick;
    QString m_desc;
    int m_sex;
    QString m_icon;
};

struct AddFriendApply {
    AddFriendApply(int from_uid, QString name, QString desc, QString icon, QString nick, int sex);
    int m_from_uid;
    QString m_name;
    QString m_desc;
    QString m_icon;
    QString m_nick;
    int m_sex;
};

struct ApplyInfo {
    ApplyInfo(int uid, QString name, QString desc,QString icon, QString nick, int sex, int status);
    ApplyInfo(std::shared_ptr<AddFriendApply> addinfo);
    void SetIcon(QString head);

    int m_uid;
    QString m_name;
    QString m_desc;
    QString m_icon;
    QString m_nick;
    int m_sex;
    int m_status;
};

struct AuthInfo {
    AuthInfo(int uid, QString name,QString nick, QString icon, int sex);

    int m_uid;
    QString m_name;
    QString m_nick;
    QString m_icon;
    int m_sex;
};

struct AuthRsp {
    AuthRsp(int peer_uid, QString peer_name, QString peer_nick, QString peer_icon, int peer_sex);

    int m_uid;
    QString m_name;
    QString m_nick;
    QString m_icon;
    int m_sex;
};

struct TextChatData{
    TextChatData(QString msg_id, QString msg_content, int fromuid, int touid);

    QString m_msg_id;
    QString m_msg_content;
    int m_from_uid;
    int m_to_uid;
};

struct TextChatMsg{
    TextChatMsg(int fromuid, int touid, QJsonArray arrays);

    int m_to_uid;
    int m_from_uid;
    std::vector<std::shared_ptr<TextChatData>> m_chat_msgs;
};

struct FriendInfo {
    FriendInfo(int uid, QString name, QString nick, QString icon,int sex,
               QString desc, QString back, QString last_msg="");
    FriendInfo(std::shared_ptr<AuthInfo> auth_info);
    FriendInfo(std::shared_ptr<AuthRsp> auth_rsp);
    void AppendChatMsgs(const std::vector<std::shared_ptr<TextChatData>> text_vec);

    int m_uid;
    QString m_name;
    QString m_nick;
    QString m_icon;
    int m_sex;
    QString m_desc;
    QString m_back;
    QString m_last_msg;
    std::vector<std::shared_ptr<TextChatData>> m_chat_msgs;
};

struct UserInfo {
    UserInfo(int uid, QString name, QString nick, QString icon, int sex, QString last_msg = "");
    UserInfo(std::shared_ptr<AuthInfo> auth);
    UserInfo(int uid, QString name, QString icon);
    UserInfo(std::shared_ptr<AuthRsp> auth);
    UserInfo(std::shared_ptr<SearchInfo> search_info);
    UserInfo(std::shared_ptr<FriendInfo> friend_info);

    int m_uid;
    QString m_name;
    QString m_nick;
    QString m_icon;
    int m_sex;
    QString m_last_msg;
    std::vector<std::shared_ptr<TextChatData>> m_chat_msgs;
};

#endif
