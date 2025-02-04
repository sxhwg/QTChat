#ifndef USERMGR_H
#define USERMGR_H

#include <QObject>
#include <vector>
#include <memory>
#include "Singleton.h"
#include "userdata.h"


class UserMgr:public QObject, public Singleton<UserMgr>,
                public std::enable_shared_from_this<UserMgr>
{
    Q_OBJECT
public:
    ~ UserMgr();

    //设置和获取用户信息的方法
    void SetUserInfo(std::shared_ptr<UserInfo> user_info);
    void SetToken(QString token);
    int GetUid();
    QString GetName();
    QString GetIcon();
    std::shared_ptr<UserInfo> GetUserInfo();

    //管理好友申请列表的方法
    void AppendApplyList(QJsonArray array);
    void AppendFriendList(QJsonArray array);
    std::vector<std::shared_ptr<ApplyInfo>> GetApplyList();
    void AddApplyList(std::shared_ptr<ApplyInfo> app);
    bool AlreadyApply(int uid);

    //获取分页聊天列表和联系人列表的方法
    std::vector<std::shared_ptr<FriendInfo>> GetChatListPerPage();
    bool IsLoadChatFin();
    void UpdateChatLoadedCount();
    std::vector<std::shared_ptr<FriendInfo>> GetConListPerPage();
    void UpdateContactLoadedCount();
    bool IsLoadConFin();

    //检查是否为好友及添加好友的方法
    bool CheckFriendById(int uid);
    void AddFriend(std::shared_ptr<AuthRsp> auth_rsp);
    void AddFriend(std::shared_ptr<AuthInfo> auth_info);
    std::shared_ptr<FriendInfo> GetFriendById(int uid);

    //添加好友聊天消息
    void AppendFriendChatMsg(int friend_id,std::vector<std::shared_ptr<TextChatData>>);
private:
    friend class Singleton<UserMgr>;
    UserMgr();

    std::shared_ptr<UserInfo> m_user_info;
    std::vector<std::shared_ptr<ApplyInfo>> m_apply_list;
    std::vector<std::shared_ptr<FriendInfo>> m_friend_list;
    QMap<int, std::shared_ptr<FriendInfo>> m_friend_map;
    QString m_token;
    int m_chat_loaded;
    int m_contact_loaded;

public slots:
    void SlotAddFriendRsp(std::shared_ptr<AuthRsp> rsp);
    void SlotAddFriendAuth(std::shared_ptr<AuthInfo> auth);
};

#endif // USERMGR_H

