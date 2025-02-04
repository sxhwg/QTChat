#include "friendlabel.h"
#include "ui_friendlabel.h"

FriendLabel::FriendLabel(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::FriendLabel)
{
    ui->setupUi(this);
    ui->close_lb->SetState("normal","hover","pressed",
                           "selected_normal","selected_hover","selected_pressed");
    connect(ui->close_lb, &ClickedLabel::clicked, this, &FriendLabel::slot_close);
}

FriendLabel::~FriendLabel()
{
    delete ui;
}

void FriendLabel::SetText(QString text)
{
    m_text = text;
    ui->tip_lb->setText(m_text);
    ui->tip_lb->adjustSize();

    QFontMetrics fontMetrics(ui->tip_lb->font()); // 获取QLabel控件的字体信息
    auto textWidth = fontMetrics.width(ui->tip_lb->text()); // 获取文本的宽度
    auto textHeight = fontMetrics.height(); // 获取文本的高度
    this->setFixedWidth(ui->tip_lb->width()+ui->close_lb->width()+5);
    this->setFixedHeight(textHeight+2);
    m_width = this->width();
    m_height = this->height();
}

int FriendLabel::Width()
{
    return m_width;
}

int FriendLabel::Height()
{
    return m_height;
}

QString FriendLabel::Text()
{
    return m_text;
}

void FriendLabel::slot_close()
{
    emit sig_close(m_text);
}
