#ifndef CLICKEDBTN_H
#define CLICKEDBTN_H

#include <QPushButton>

class ClickedBtn:public QPushButton
{
    Q_OBJECT

public:
    explicit ClickedBtn(QWidget *parent = nullptr);
    ~ClickedBtn();
    void SetState(QString normal, QString hover, QString press);

protected:
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QString m_normal;
    QString m_hover;
    QString m_press;
};

#endif // CLICKEDBTN_H
