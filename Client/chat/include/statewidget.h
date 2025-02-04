#ifndef STATEWIDGET_H
#define STATEWIDGET_H

#include <QWidget>
#include <QLabel>
#include "global.h"

class StateWidget : public QWidget
{
    Q_OBJECT

public:
    explicit StateWidget(QWidget *parent = nullptr);

    void SetState(QString normal="", QString hover="", QString press="",
                  QString select="", QString select_hover="", QString select_press="");
    ClickLbState GetCurState();
    void ClearState();
    void SetSelected(bool bselected);
    void AddRedPoint();
    void ShowRedPoint(bool show = true);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent *ev) override;
    void mouseReleaseEvent(QMouseEvent *ev) override;
    void enterEvent(QEvent* event) override;
    void leaveEvent(QEvent* event) override;

private:
    QString m_normal;
    QString m_normal_hover;
    QString m_normal_press;
    QString m_selected;
    QString m_selected_hover;
    QString m_selected_press;
    ClickLbState m_curstate;
    QLabel * m_red_point;

signals:
    void clicked();
};

#endif // STATEWIDGET_H
