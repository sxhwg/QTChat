#ifndef AUTHENFRIEND_H
#define AUTHENFRIEND_H

#include <QDialog>
#include "clickedlabel.h"
#include "userdata.h"
#include "friendlabel.h"

namespace Ui {
class AuthenFriend;
}

class AuthenFriend : public QDialog
{
    Q_OBJECT

public:
    explicit AuthenFriend(QWidget *parent = nullptr);
    ~AuthenFriend();

    void InitTipLbs();
    void AddTipLbs(ClickedLabel *lb, QPoint cur_point, QPoint &next_point, int text_width, int text_height);
    bool eventFilter(QObject *obj, QEvent *event) override;
    void SetApplyInfo(std::shared_ptr<ApplyInfo> apply_info);

public slots:
    //显示更多label标签
    void ShowMoreLabel();
    //输入label按下回车触发将标签加入展示栏
    void SlotLabelEnter();
    //点击关闭，移除展示栏好友便签
    void SlotRemoveFriendLabel(QString name);
    //通过点击tip实现增加和减少好友便签
    void SlotChangeFriendLabelByTip(QString lbtext, ClickLbState state);
    //输入框文本变化显示不同提示
    void SlotLabelTextChange(const QString& text);
    //输入框输入完成
    void SlotLabelEditFinished();
    //输入标签显示提示框，点击提示框内容后添加好友便签
    void SlotAddFirendLabelByClickTip(QString text);
    //处理确认回调
    void SlotApplySure();
    //处理取消回调
    void SlotApplyCancel();

private:
    void resetLabels();
    void addLabel(QString name);

    //已经创建好的标签
    QMap<QString, ClickedLabel*> m_add_labels;
    std::vector<QString> m_add_label_keys;
    QPoint m_label_point;
    //用来在输入框显示添加新好友的标签
    QMap<QString, FriendLabel*> m_friend_labels;
    std::vector<QString> m_friend_label_keys;
    std::vector<QString> m_tip_data;
    QPoint m_tip_cur_point;
    std::shared_ptr<ApplyInfo> m_apply_info;
    Ui::AuthenFriend *ui;
};

#endif // AUTHENFRIEND_H
