#include "applyfrienditem.h"
#include "ui_applyfrienditem.h"

ApplyFriendItem::ApplyFriendItem(QWidget *parent)
    : ListItemBase(parent)
    , ui(new Ui::ApplyFriendItem), m_added(false)
{
    ui->setupUi(this);
    SetItemType(ListItemType::APPLY_FRIEND_ITEM);
    ui->addBtn->SetState("normal","hover", "press");
    ui->addBtn->hide();
    connect(ui->addBtn, &ClickedBtn::clicked,  [this](){
        emit this->sig_auth_friend(m_apply_info);
    });
}

ApplyFriendItem::~ApplyFriendItem()
{
    delete ui;
}

void ApplyFriendItem::SetInfo(std::shared_ptr<ApplyInfo> apply_info)
{
    m_apply_info = apply_info;
    // 加载图片
    QPixmap pixmap(m_apply_info->m_icon);

    // 设置图片自动缩放
    ui->icon_lb->setPixmap(pixmap.scaled(ui->icon_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_lb->setScaledContents(true);

    ui->user_name_lb->setText(m_apply_info->m_name);
    ui->user_chat_lb->setText(m_apply_info->m_desc);
}

void ApplyFriendItem::ShowAddBtn(bool bshow)
{
    if (bshow) {
        ui->addBtn->show();
        ui->already_add_lb->hide();
        m_added = false;
    }
    else {
        ui->addBtn->hide();
        ui->already_add_lb->show();
        m_added = true;
    }
}

QSize ApplyFriendItem::sizeHint() const
{
    return QSize(250, 80); // 返回自定义的尺寸
}

int ApplyFriendItem::GetUid() {
    return m_apply_info->m_uid;
}
