#ifndef CHATUSERLIST_H
#define CHATUSERLIST_H

#include <QListWidget>

class ChatUserList: public QListWidget
{
    Q_OBJECT

public:
    explicit ChatUserList(QWidget *parent = nullptr);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    bool m_load_pending;

signals:
    void sig_loading_chat_user();
};

#endif // CHATUSERLIST_H
