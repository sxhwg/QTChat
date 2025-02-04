#include "chatuserwid.h"
#include "ui_chatuserwid.h"

ChatUserWid::ChatUserWid(QWidget *parent)
    : ListItemBase(parent)
    , ui(new Ui::ChatUserWid)
{
    ui->setupUi(this);

    SetItemType(ListItemType::CHAT_USER_ITEM);
    ui->red_point->raise();
    ShowRedPoint(false);
}

ChatUserWid::~ChatUserWid()
{
    delete ui;
}

QSize ChatUserWid::sizeHint() const
{
    return QSize(250, 70);
}

void ChatUserWid::SetInfo(std::shared_ptr<UserInfo> user_info)
{
    m_user_info = user_info;
    // 加载图片
    QPixmap pixmap(m_user_info->m_icon);

    // 设置图片自动缩放
    ui->icon_lb->setPixmap(pixmap.scaled(ui->icon_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_lb->setScaledContents(true);

    ui->user_name_lb->setText(m_user_info->m_name);
    ui->user_chat_lb->setText(m_user_info->m_last_msg);
}

void ChatUserWid::SetInfo(std::shared_ptr<FriendInfo> friend_info)
{
    m_user_info = std::make_shared<UserInfo>(friend_info);
    // 加载图片
    QPixmap pixmap(m_user_info->m_icon);

    // 设置图片自动缩放
    ui->icon_lb->setPixmap(pixmap.scaled(ui->icon_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_lb->setScaledContents(true);

    ui->user_name_lb->setText(m_user_info->m_name);
    ui->user_chat_lb->setText(m_user_info->m_last_msg);
}

void ChatUserWid::ShowRedPoint(bool bshow)
{
    if(bshow){
        ui->red_point->show();
    }else{
        ui->red_point->hide();
    }
}

std::shared_ptr<UserInfo> ChatUserWid::GetUserInfo()
{
    return m_user_info;
}

void ChatUserWid::updateLastMsg(std::vector<std::shared_ptr<TextChatData>> msgs)
{
    QString last_msg = "";
    for (auto& msg : msgs) {
        last_msg = msg->m_msg_content;
        m_user_info->m_chat_msgs.push_back(msg);
    }

    m_user_info->m_last_msg = last_msg;
    ui->user_chat_lb->setText(m_user_info->m_last_msg);
}
