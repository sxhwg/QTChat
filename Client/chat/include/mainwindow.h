#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "logindialog.h"
#include "registerdialog.h"
#include "resetdialog.h"
#include "chatdialog.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void SlotSwitchReg();
    void SlotSwitchLogin();
    void SlotSwitchReset();
    void SlotSwitchChat();

private:
    Ui::MainWindow *ui;
    LoginDialog *m_login_dlg;
    RegisterDialog *m_reg_dlg;
    ResetDialog *m_reset_dlg;
    ChatDialog *m_chat_dlg;
};
#endif // MAINWINDOW_H
