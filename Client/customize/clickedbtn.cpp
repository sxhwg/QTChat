#include "clickedbtn.h"
#include <QVariant>
#include "global.h"


ClickedBtn::ClickedBtn(QWidget *parent):QPushButton (parent)
{
    setCursor(Qt::PointingHandCursor); // 设置光标为小手
    setFocusPolicy(Qt::NoFocus);
}

ClickedBtn::~ClickedBtn()
{

}

void ClickedBtn::SetState(QString normal, QString hover, QString press)
{
    m_hover = hover;
    m_normal = normal;
    m_press = press;
    setProperty("state", m_normal);
    repolish(this);
    update();
}

void ClickedBtn::enterEvent(QEvent *event)
{
    setProperty("state", m_hover);
    repolish(this);
    update();
    QPushButton::enterEvent(event);
}

void ClickedBtn::leaveEvent(QEvent *event)
{
    setProperty("state", m_normal);
    repolish(this);
    update();
    QPushButton::leaveEvent(event);
}

void ClickedBtn::mousePressEvent(QMouseEvent *event)
{
    setProperty("state", m_press);
    repolish(this);
    update();
    QPushButton::mousePressEvent(event);
}

void ClickedBtn::mouseReleaseEvent(QMouseEvent *event)
{
    setProperty("state", m_hover);
    repolish(this);
    update();
    QPushButton::mouseReleaseEvent(event);
}
