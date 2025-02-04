#ifndef TCPMGR_H
#define TCPMGR_H

#include <QObject>
#include <QTcpSocket>
#include "singleton.h"
#include "global.h"
#include "userdata.h"

class TcpMgr:public QObject, public Singleton<TcpMgr>,
               public std::enable_shared_from_this<TcpMgr>
{
    Q_OBJECT
public:
    ~ TcpMgr();

public slots:
    void slot_tcp_connect(ServerInfo si);

private slots:
    void slot_send_data(ReqId reqId, QByteArray data);

private:
    friend class Singleton<TcpMgr>;
    TcpMgr();
    void initHandlers();
    void handleMsg(ReqId id, int len, QByteArray data);

    QMap<ReqId, std::function<void(ReqId id, int len, QByteArray data)>> m_handlers;
    QTcpSocket m_socket;
    QString m_host;
    uint16_t m_port;
    QByteArray m_buffer;
    bool m_recv_pending;
    quint16 m_message_id;
    quint16 m_message_len;

signals:
    void sig_con_success(bool bsuccess);
    void sig_send_data(ReqId reqId, QByteArray data);
    void sig_swich_chatdlg();
    void sig_load_apply_list(QJsonArray json_array);
    void sig_login_failed(int);
    void sig_user_search(std::shared_ptr<SearchInfo>);
    void sig_friend_apply(std::shared_ptr<AddFriendApply>);
    void sig_add_auth_friend(std::shared_ptr<AuthInfo>);
    void sig_auth_rsp(std::shared_ptr<AuthRsp>);
    void sig_text_chat_msg(std::shared_ptr<TextChatMsg> msg);
};

#endif // TCPMGR_H
