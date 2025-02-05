#ifndef TIMERBTN_H
#define TIMERBTN_H

#include <QPushButton>
#include <QTimer>

class TimerBtn : public QPushButton
{
public:
    explicit TimerBtn(QWidget *parent = nullptr);
    ~ TimerBtn();

protected:
    virtual void mouseReleaseEvent(QMouseEvent *e) override;

private:
    QTimer *m_timer;
    int m_counter;
};

#endif // TIMERBTN_H
