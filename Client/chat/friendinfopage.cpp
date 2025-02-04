#include "friendinfopage.h"
#include "ui_friendinfopage.h"

FriendInfoPage::FriendInfoPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FriendInfoPage), m_user_info(nullptr)
{
    ui->setupUi(this);

    ui->msg_chat->SetState("normal","hover","press");
    ui->video_chat->SetState("normal","hover","press");
    ui->voice_chat->SetState("normal","hover","press");
}

FriendInfoPage::~FriendInfoPage()
{
    delete ui;
}

void FriendInfoPage::SetInfo(std::shared_ptr<UserInfo> user_info)
{
    m_user_info = user_info;
    // 加载图片
    QPixmap pixmap(user_info->m_icon);

    // 设置图片自动缩放
    ui->icon_lb->setPixmap(pixmap.scaled(ui->icon_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_lb->setScaledContents(true);

    ui->name_lb->setText(user_info->m_name);
    ui->nick_lb->setText(user_info->m_nick);
    ui->bak_lb->setText(user_info->m_nick);
}

void FriendInfoPage::on_msg_chat_clicked()
{
    qDebug() << "msg chat btn clicked";
    emit sig_jump_chat_item(m_user_info);
}
