#ifndef CLICKEDONCELABEL_H
#define CLICKEDONCELABEL_H

#include <QLabel>

class ClickedOnceLabel:public QLabel
{
    Q_OBJECT

public:
    ClickedOnceLabel(QWidget *parent = nullptr);

protected:
    void mouseReleaseEvent(QMouseEvent *ev) override;

signals:
    void clicked(QString );
};

#endif // CLICKEDONCELABEL_H
