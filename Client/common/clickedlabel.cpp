#include "clickedlabel.h"
#include <QMouseEvent>

ClickedLabel::ClickedLabel(QWidget* parent):QLabel (parent), m_curstate(ClickLbState::Normal)
{
    setCursor(Qt::PointingHandCursor);
}

void ClickedLabel::SetState(QString normal, QString normal_hover, QString normal_press,
                            QString selected, QString selected_hover, QString selected_press)
{
    m_normal = normal;
    m_normal_hover = normal_hover;
    m_normal_press = normal_press;
    m_selected = selected;
    m_selected_hover = selected_hover;
    m_selected_press = selected_press;

    setProperty("state",normal);
    repolish(this);
}

ClickLbState ClickedLabel::GetCurState()
{
    return m_curstate;
}

bool ClickedLabel::SetCurState(ClickLbState state)
{
    m_curstate = state;
    if (m_curstate == ClickLbState::Normal) {
        setProperty("state", m_normal);
        repolish(this);
    }
    else if (m_curstate == ClickLbState::Selected) {
        setProperty("state", m_selected);
        repolish(this);
    }

    return true;
}

void ClickedLabel::ResetNormalState()
{
    m_curstate = ClickLbState::Normal;
    setProperty("state", m_normal);
    repolish(this);
}

void ClickedLabel::mousePressEvent(QMouseEvent *ev)
{
    if (ev->button() == Qt::LeftButton) {
        if(m_curstate == ClickLbState::Normal){
            m_curstate = ClickLbState::Selected;
            setProperty("state",m_selected_press);
            repolish(this);
            update();
        }else{
            m_curstate = ClickLbState::Normal;
            setProperty("state",m_normal_press);
            repolish(this);
            update();
        }
        return;
    }
    QLabel::mousePressEvent(ev);
}

void ClickedLabel::mouseReleaseEvent(QMouseEvent *ev)
{
    if (ev->button() == Qt::LeftButton) {
        if(m_curstate == ClickLbState::Normal){
            setProperty("state",m_normal_hover);
            repolish(this);
            update();
        }else{
            setProperty("state",m_selected_hover);
            repolish(this);
            update();
        }
        emit clicked(this->text(), m_curstate);
        return;
    }
    QLabel::mousePressEvent(ev);
}

void ClickedLabel::enterEvent(QEvent *ev)
{
    if(m_curstate == ClickLbState::Normal){
        setProperty("state",m_normal_hover);
        repolish(this);
        update();
    }else{
        setProperty("state",m_selected_hover);
        repolish(this);
        update();
    }
    QLabel::enterEvent(ev);
}

void ClickedLabel::leaveEvent(QEvent *ev)
{
    if(m_curstate == ClickLbState::Normal){
        setProperty("state",m_normal);
        repolish(this);
        update();
    }else{
        setProperty("state",m_selected);
        repolish(this);
        update();
    }
    QLabel::leaveEvent(ev);
}
