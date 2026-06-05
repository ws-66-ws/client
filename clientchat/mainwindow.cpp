#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tcpmgr.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    _login_wd = new LoginWidget(this);
    // 并将_login_wd设置为widget的中心部件
    setCentralWidget(_login_wd);
    _login_wd->show();

    // 连接注册信号(登录转注册界面)
    connect(_login_wd, &LoginWidget::switchRegister, this, &MainWindow::slotSwitchReg);

    //连接创建聊天界面信号
    connect(TcpMgr::getInstance().get(), &TcpMgr::sig_swich_chatwd,
            this, &MainWindow::slotSwitchChat);
    //链接服务器踢人消息
    connect(TcpMgr::getInstance().get(),&TcpMgr::sig_notify_offline, this, &MainWindow::slotOffline);
    //连接服务器断开心跳超时或异常连接信息
    connect(TcpMgr::getInstance().get(),&TcpMgr::sig_connection_closed, this, &MainWindow::slotExcepConOffline);
    // //用于测试
    // emit TcpMgr::getInstance()->sig_swich_chatwd();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slotSwitchReg()
{
    _register_wd = new RegisterWidget(this);
    setCentralWidget(_register_wd);
    _register_wd->show();

    // 连接登录信号(注册转登录界面)
    connect(_register_wd, &RegisterWidget::sig_cancel, this, &MainWindow::slotSwitchLog);
    _ui_status = REGISTER_UI;
}

void MainWindow::slotSwitchLog()
{
    _login_wd = new LoginWidget(this);
    setCentralWidget(_login_wd);
    _login_wd->show();

    // 连接注册信号(登录转注册界面)
    connect(_login_wd, &LoginWidget::switchRegister, this, &MainWindow::slotSwitchReg);
    _ui_status = LOGIN_UI;
}

void MainWindow::slotSwitchChat()
{
    _chat_wd = new ChatWidget(this);
    setCentralWidget(_chat_wd);
    _chat_wd->show();
    this->setMinimumSize(QSize(1000, 700));
    this->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);

    _ui_status = CHAT_UI;
    _chat_wd->loadChatList();
}

void MainWindow::slotOffline()
{
    // 使用静态方法直接弹出一个信息框
    QMessageBox::information(this, "下线提示", "同账号异地登录，该终端下线！");
    TcpMgr::getInstance()->closeConnection();
    offlineLogin();
}

void MainWindow::slotExcepConOffline()
{
    // 使用静态方法直接弹出一个信息框
    QMessageBox::information(this, "下线提示", "心跳超时或临界异常，该终端下线！");
    TcpMgr::getInstance()->closeConnection();
    offlineLogin();
}

void MainWindow::offlineLogin()
{
    if(_ui_status == LOGIN_UI){
        return;
    }

    _login_wd = new LoginWidget(this);
    setCentralWidget(_login_wd);

    this->setMaximumSize(360, 460);
    this->setMinimumSize(360, 460);
    this->resize(360, 460);
    _login_wd->show();

    // 连接注册信号(登录转注册界面)
    connect(_login_wd, &LoginWidget::switchRegister, this, &MainWindow::slotSwitchReg);
    _ui_status = LOGIN_UI;
}
