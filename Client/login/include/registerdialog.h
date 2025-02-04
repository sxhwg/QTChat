#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <QDialog>
#include <QMap>
#include "global.h"

namespace Ui {
class RegisterDialog;
}

class RegisterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RegisterDialog(QWidget *parent = nullptr);
    ~RegisterDialog();

private slots:
    void slot_reg_mod_finish(ReqId id, QByteArray res, ErrorCodes err);
    void on_get_code_clicked();
    void on_sure_btn_clicked();
    void on_cancel_btn_clicked();
    void on_return_btn_clicked();

private:
    void initHttpHandlers();
    void ChangeTipPage();
    void showTip(QString str, bool flag);
    void AddTipErr(TipErr te,QString tips);
    void DelTipErr(TipErr te);
    bool checkUserValid();
    bool checkEmailValid();
    bool checkPassValid();
    bool checkConfirmValid();
    bool checkVerifyValid();

    Ui::RegisterDialog *ui;
    QMap<ReqId, std::function<void(const QJsonObject&)>> m_handlers;
    QTimer *m_countdown_timer;
    int m_countdown;
    QMap<TipErr, QString> m_tip_errs;

signals:
    void sigSwitchLogin();
};

#endif // REGISTERDIALOG_H
