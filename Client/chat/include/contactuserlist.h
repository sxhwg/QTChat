#ifndef CONTACTUSERLIST_H
#define CONTACTUSERLIST_H

#include <QListWidget>
#include "userdata.h"

class ConUserItem;

class ContactUserList : public QListWidget
{
    Q_OBJECT
public:
    ContactUserList(QWidget* parent = nullptr);

    void ShowRedPoint(bool bshow = true);

public slots:
    void slot_item_clicked(QListWidgetItem *item);
    void slot_add_auth_firend(std::shared_ptr<AuthInfo> auth_info);
    void slot_auth_rsp(std::shared_ptr<AuthRsp>);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void addContactUserList();

private:
    bool m_load_pending;
    ConUserItem* m_add_friend_item;
    QListWidgetItem * m_groupitem;

signals:
    void sig_loading_contact_user();
    void sig_switch_apply_friend_page();
    void sig_switch_friend_info_page(std::shared_ptr<UserInfo> user_info);
};

#endif // CONTACTUSERLIST_H
