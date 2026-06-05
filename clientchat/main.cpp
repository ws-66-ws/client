#include "mainwindow.h"

#include <QApplication>
#include <QDir>
#include <QSettings>


int main(int argc, char *argv[])
{

    QApplication a(argc, argv);

    // 获取当前应用程序的路径
    QString appPath = QCoreApplication::applicationDirPath();
    // 拼接文件名
    QString fileName = "config.ini";
    QString configPath = QDir::toNativeSeparators(appPath + QDir::separator() + fileName);
    QSettings settings(configPath, QSettings::IniFormat);
    QString gateHost = settings.value("GateServer/host").toString();
    QString gatePort = settings.value("GateServer/port").toString();

    gate_url_prefix = "http://"+gateHost+":"+gatePort;

    MainWindow w;
    w.show();
    return a.exec();
}
