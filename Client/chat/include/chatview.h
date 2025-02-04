#ifndef CHATVIEW_H
#define CHATVIEW_H

#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>

class ChatView : public QWidget
{
    Q_OBJECT

public:
    explicit ChatView(QWidget *parent = nullptr);

    void appendChatItem(QWidget *item);                 //尾插
    void prependChatItem(QWidget *item);                //头插
    void insertChatItem(QWidget *before, QWidget *item);//中间插
    void removeAllItem();

protected:
    bool eventFilter(QObject *o, QEvent *e) override;
    void paintEvent(QPaintEvent *event) override;

private slots:
    void onVScrollBarMoved(int min, int max);

private:
    QVBoxLayout *m_pVl;
    QScrollArea *m_pScrollArea;
    bool isAppended;
};

#endif // CHATVIEW_H
