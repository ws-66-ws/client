#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "loginwidget.h"
#include "registerwidget.h"
#include "chatwidget.h"

namespace Ui {
class MainWindow;
}

enum UIStatus{
    LOGIN_UI,
    REGISTER_UI,
    RESET_UI,
    CHAT_UI
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // 选择注册
    void slotSwitchReg();

    // 选择登录
    void slotSwitchLog();

    // 选择聊天框
    void slotSwitchChat();

    // 下线
    void slotOffline();

    // 心跳检测超时下线
    void slotExcepConOffline();

private:
    void offlineLogin();

private:
    Ui::MainWindow *ui;

    LoginWidget *_login_wd;

    RegisterWidget *_register_wd;

    ChatWidget *_chat_wd;

    UIStatus _ui_status;
};

#endif // MAINWINDOW_H
