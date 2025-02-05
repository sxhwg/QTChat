#ifndef CLICKEDLABEL_H
#define CLICKEDLABEL_H

#include <QLabel>
#include "global.h"

class ClickedLabel:public QLabel
{
    Q_OBJECT

public:
    explicit ClickedLabel(QWidget* parent);
    void SetState(QString normal="", QString normal_hover="", QString normal_press="",
                  QString selected="", QString selected_hover="", QString selected_press="");
    ClickLbState GetCurState();
    bool SetCurState(ClickLbState state);
    void ResetNormalState();

protected:
    void mousePressEvent(QMouseEvent *ev) override;
    void mouseReleaseEvent(QMouseEvent *ev) override;
    void enterEvent(QEvent* ev) override;
    void leaveEvent(QEvent* ev) override;

private:
    QString m_normal;
    QString m_normal_hover;
    QString m_normal_press;
    QString m_selected;
    QString m_selected_hover;
    QString m_selected_press;
    ClickLbState m_curstate;

signals:
    void clicked(QString, ClickLbState);
};

#endif // CLICKEDLABEL_H
