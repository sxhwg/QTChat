#include "mainwindow.h"
#include <QApplication>
#include <QFile>
#include <QDebug>
#include <QDir>
#include <QSettings>
#include "global.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //设置程序图标
    a.setWindowIcon(QIcon(":/res/icon.ico"));

    //读取并设置QSS样式表
    QFile qss(":/res/stylesheet.qss");
    if(qss.open(QFile::ReadOnly | QFile::Text))
    {
        qDebug() << "open stylesheet success";
        QString style = QLatin1String(qss.readAll());
        a.setStyleSheet(style);
        qss.close();
    }else
    {
        qDebug() << "Open stylesheet failed";
    }

    // 获取当前应用程序的路径
    QString app_path = QCoreApplication::applicationDirPath();
    // 拼接配置文件路径
    QString fileName = "config.ini";
    QString config_path = QDir::toNativeSeparators(app_path + QDir::separator() + fileName);
    //读取配置文件
    QSettings settings(config_path, QSettings::IniFormat);
    QString gate_host = settings.value("GateServer/host").toString();
    QString gate_port = settings.value("GateServer/port").toString();
    gate_url_prefix = "http://"+gate_host+":"+gate_port;

    MainWindow w;
    w.show();
    return a.exec();
}
