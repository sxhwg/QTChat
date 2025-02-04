#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QMap>
#include "global.h"

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

private slots:
    void slot_login_mod_finish(ReqId id, QByteArray res, ErrorCodes err);
    void slot_forget_pwd();
    void on_login_btn_clicked();
    void slot_tcp_con_finish(bool bsuccess);
    void slot_login_failed(int err);

private:
    void initHttpHandlers();
    void showTip(QString str, bool b_ok);
    void AddTipErr(TipErr te, QString tips);
    void DelTipErr(TipErr te);
    bool checkEmailValid();
    bool checkPwdValid();
    bool enableBtn(bool flag);
    void initHead();

    Ui::LoginDialog *ui;
    QMap<ReqId, std::function<void(const QJsonObject&)>> m_handlers;
    int m_uid;
    QString m_token;
    QMap<TipErr, QString> m_tip_errs;

signals:
    void switchRegister();
    void switchReset();
    void sig_connect_tcp(ServerInfo si);
};

#endif // LOGINDIALOG_H
