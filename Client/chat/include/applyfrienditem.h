#ifndef APPLYFRIENDITEM_H
#define APPLYFRIENDITEM_H

#include "listitembase.h"
#include "userdata.h"

namespace Ui {
class ApplyFriendItem;
}

class ApplyFriendItem : public ListItemBase
{
    Q_OBJECT

public:
    explicit ApplyFriendItem(QWidget *parent = nullptr);
    ~ApplyFriendItem();

    void SetInfo(std::shared_ptr<ApplyInfo> apply_info);
    void ShowAddBtn(bool bshow);
    QSize sizeHint() const override;
    int GetUid();

private:
    Ui::ApplyFriendItem *ui;
    std::shared_ptr<ApplyInfo> m_apply_info;
    bool m_added;

signals:
    void sig_auth_friend(std::shared_ptr<ApplyInfo> apply_info);
};

#endif // APPLYFRIENDITEM_H
