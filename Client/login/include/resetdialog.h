#ifndef RESETDIALOG_H
#define RESETDIALOG_H

#include <QDialog>
#include <QMap>
#include "global.h"

namespace Ui {
class ResetDialog;
}

class ResetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ResetDialog(QWidget *parent = nullptr);
    ~ResetDialog();

private slots:
    void slot_reset_mod_finish(ReqId id, QByteArray res, ErrorCodes err);
    void on_verify_btn_clicked();
    void on_sure_btn_clicked();
    void on_return_btn_clicked();

private:
    void initHandlers();
    void showTip(QString str,bool b_ok);
    void AddTipErr(TipErr te,QString tips);
    void DelTipErr(TipErr te);
    bool checkUserValid();
    bool checkEmailValid();
    bool checkPassValid();
    bool checkVerifyValid();

    Ui::ResetDialog *ui;
    QMap<TipErr, QString> m_tip_errs;
    QMap<ReqId, std::function<void(const QJsonObject&)>> m_handlers;

signals:
    void switchLogin();
};

#endif // RESETDIALOG_H
