#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tcpmgr.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //创建登录窗口对象，修改主窗口标志，将登录窗口设置为主窗口的的中心窗口
    m_login_dlg = new LoginDialog(this);
    m_login_dlg->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    setCentralWidget(m_login_dlg);
    //接收登录界面注册信号，调用切换注册界面槽函数
    connect(m_login_dlg, &LoginDialog::switchRegister, this, &MainWindow::SlotSwitchReg);
    //连接登录界面忘记密码信号，调用切换重置界面槽函数
    connect(m_login_dlg, &LoginDialog::switchReset, this, &MainWindow::SlotSwitchReset);

    //连接创建聊天界面信号
    connect(TcpMgr::GetInstance().get(),&TcpMgr::sig_swich_chatdlg, this, &MainWindow::SlotSwitchChat);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::SlotSwitchReg()
{   m_reg_dlg = new RegisterDialog(this);
    m_reg_dlg->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    setCentralWidget(m_reg_dlg);
    //接收注册界面返回信号，调用切换登录界面槽函数
    connect(m_reg_dlg, &RegisterDialog::sigSwitchLogin, this, &MainWindow::SlotSwitchLogin);
}

void MainWindow::SlotSwitchLogin()
{
    m_login_dlg = new LoginDialog(this);
    m_login_dlg->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    setCentralWidget(m_login_dlg);
    connect(m_login_dlg, &LoginDialog::switchRegister, this, &MainWindow::SlotSwitchReg);
    connect(m_login_dlg, &LoginDialog::switchReset, this, &MainWindow::SlotSwitchReset);
}

void MainWindow::SlotSwitchReset()
{
    m_reset_dlg = new ResetDialog(this);
    m_reset_dlg->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    setCentralWidget(m_reset_dlg);
    //接收重置密码界面返回信号，调用切换登录界面槽函数
    connect(m_reset_dlg, &ResetDialog::switchLogin, this, &MainWindow::SlotSwitchLogin);
}

void MainWindow::SlotSwitchChat()
{
    m_chat_dlg = new ChatDialog(this);
    m_chat_dlg->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    setCentralWidget(m_chat_dlg);
    this->setMinimumSize(QSize(1050,900));
    this->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
}
