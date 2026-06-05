#ifndef LOGINWIDGET_H
#define LOGINWIDGET_H

#include <QWidget>
#include "global.h"

namespace Ui {
class LoginWidget;
}

class LoginWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LoginWidget(QWidget *parent = nullptr);
    ~LoginWidget();

private:
    // 初始化处理 http 的函数
    void initHttpHandlers();

    bool checkEmailValid();

    bool checkPwdValid();

signals:
    // 注册信号
    void switchRegister();

    void sig_tcp_connect(ServerInfo);

private slots:
    // 登录
    void on_login_Button_clicked();

    void slot_login_mod_finish(ReqId id, QString res, ErrorCodes err);

    void slot_tcp_con_finish(bool bsuccess);

    void sig_login_failed(int err);

private:
    Ui::LoginWidget *ui;

    QMap<ReqId, std::function<void(const QJsonObject &)>> _handlers;

    int _uid;

    QString _token;
};

#endif // LOGINWIDGET_H
