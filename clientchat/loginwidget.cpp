#include "loginwidget.h"
#include "ui_loginwidget.h"

#include <QJsonObject>
#include "httpmgr.h"
#include "tcpmgr.h"

LoginWidget::LoginWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LoginWidget)
{
    ui->setupUi(this);

    initHttpHandlers();

    connect(ui->register_Label, &ClickableLabel::clicked, this, &LoginWidget::switchRegister);
    //连接登录回包信号
    connect(HttpMgr::getInstance().get(), &HttpMgr::sig_login_mod_finish,
            this, &LoginWidget::slot_login_mod_finish);
    //连接tcp连接请求的信号和槽函数
    connect(this, &LoginWidget::sig_tcp_connect,
            TcpMgr::getInstance().get(), &TcpMgr::slot_tcp_connect);
    //连接tcp管理者发出的连接成功信号
    connect(TcpMgr::getInstance().get(), &TcpMgr::sig_con_success,
            this, &LoginWidget::slot_tcp_con_finish);
    //连接tcp管理者发出的登陆失败信号
    connect(TcpMgr::getInstance().get(), &TcpMgr::sig_login_failed,
            this, &LoginWidget::sig_login_failed);
}

LoginWidget::~LoginWidget()
{
    delete ui;
}

void LoginWidget::initHttpHandlers()
{
    //注册获取 登录 回包逻辑
    _handlers.insert(ReqId::ID_LOGIN_USER, [this](QJsonObject jsonObj){
        int error = jsonObj["error"].toInt();
        if(error != ErrorCodes::SUCCESS){
            qDebug() << "参数错误";
            return;
        }

        auto email = jsonObj["email"].toString();
        qDebug() << "登录成功";

        //发送信号通知tcpMgr发送长链接
        ServerInfo serverInfo;
        serverInfo.Uid = jsonObj["uid"].toInt();
        serverInfo.Host = jsonObj["host"].toString();
        serverInfo.Port = jsonObj["port"].toString();
        serverInfo.Token = jsonObj["token"].toString();

        _uid = serverInfo.Uid;
        _token = serverInfo.Token;

        qDebug()<< "email is " << email << " uid is " << serverInfo.Uid <<" host is "
                << serverInfo.Host << " Port is " << serverInfo.Port << " Token is "
                << serverInfo.Token;
        emit sig_tcp_connect(serverInfo);
    });
}

bool LoginWidget::checkEmailValid()
{
    auto email = ui->email_LineEdit->text();
    if(email.isEmpty()){
        qDebug() << "email empty " ;
        return false;
    }
    return true;
}

bool LoginWidget::checkPwdValid()
{
    auto pwd = ui->password_LineEdit->text();
    if(pwd.length() < 6 || pwd.length() > 15){
        qDebug() << "Pass length invalid";
        return false;
    }
    return true;
}

void LoginWidget::on_login_Button_clicked()
{
    qDebug()<<"login btn clicked";
    if(checkEmailValid() == false){
        return;
    }
    if(checkPwdValid() == false){
        return;
    }
    auto email = ui->email_LineEdit->text();
    auto pwd = ui->password_LineEdit->text();
    //发送http请求登录
    QJsonObject jsonObj;
    jsonObj["email"] = email;
    jsonObj["password"] = pwd;
    HttpMgr::getInstance()->postHttpReq(QUrl(gate_url_prefix + "/user_login"),
                                        jsonObj, ReqId::ID_LOGIN_USER, Modules::LOGINMOD);
}

void LoginWidget::slot_login_mod_finish(ReqId id, QString res, ErrorCodes err)
{
    if(err != ErrorCodes::SUCCESS){
        qDebug() << "网络请求错误";
        return;
    }

    // 解析 JSON 字符串,res需转化为QByteArray
    QJsonDocument jsonDoc = QJsonDocument::fromJson(res.toUtf8());

    if(jsonDoc.isNull()){
        qDebug() << "json解析错误";
        return;
    }

    if(!jsonDoc.isObject()){
        qDebug() << "json解析错误";
        return;
    }

    //调用对应的逻辑,根据id回调。
    _handlers[id](jsonDoc.object());

    return;
}

void LoginWidget::slot_tcp_con_finish(bool bsuccess)
{
    if(bsuccess){
        qDebug() << "聊天服务连接成功，正在登录...";
        QJsonObject jsonObj;
        jsonObj["uid"] = _uid;
        jsonObj["token"] = _token;

        QJsonDocument doc(jsonObj);
        QString jsonString = doc.toJson(QJsonDocument::Indented);   // 序列化为缩进格式

        //发送tcp请求给chat server
        TcpMgr::getInstance()->sig_send_data(ReqId::ID_CHAT_LOGIN, jsonString);
    }
    else{
        qDebug() << "网络异常";

    }
}

void LoginWidget::sig_login_failed(int err)
{
    QString result = QString("登录失败, err is %1").arg(err);
    qDebug() << result;

}

