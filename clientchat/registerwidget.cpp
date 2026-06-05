#include "registerwidget.h"
#include "ui_registerwidget.h"

#include <QRegularExpression>
#include "httpmgr.h"
#include <QStyle>

RegisterWidget::RegisterWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RegisterWidget)
{
    ui->setupUi(this);

    initHttpHandlers();

    connect(HttpMgr::getInstance().get(), &HttpMgr::sig_reg_mod_finish,
            this, &RegisterWidget::slot_reg_mod_finish);

    connect(ui->email_LineEdit, &QLineEdit::editingFinished, [this](){
        checkEmailValid();
    });

    connect(ui->password_LineEdit, &QLineEdit::editingFinished, [this](){
        checkPassValid();
    });

    connect(ui->verify_LineEdit, &QLineEdit::editingFinished, [this](){
        checkVerifyValid();
    });
}

RegisterWidget::~RegisterWidget()
{
    delete ui;
}

void RegisterWidget::showErr(QString str, bool isSuccess)
{
    ui->err_label->setText(str);
    if (isSuccess) {
        ui->err_label->setStyleSheet("color: green;");
        ui->err_label->setProperty("state", "success");
    }
    else {
        ui->err_label->setStyleSheet("color: red;");
        ui->err_label->setProperty("state", "err");
    }
    // 刷新样式，确保更改立即生效
    ui->err_label->style()->unpolish(ui->err_label);
    ui->err_label->style()->polish(ui->err_label);
}

void RegisterWidget::initHttpHandlers()
{
    // 注册获取验证码回包逻辑
    _handlers.insert(ReqId::ID_GET_VARIFY_CODE, [this](QJsonObject jsonObj){
        int error = jsonObj["error"].toInt();
        if (error != ErrorCodes::SUCCESS) {
            showErr(tr("参数错误"), false);
            qDebug() << "error is" << error;
            return;
        }

        auto email = jsonObj["email"].toString();
        showErr(tr("验证码已发送到邮箱，注意查收"), true);
        qDebug()<< "email is " << email ;
    });

    //注册用户回包逻辑
    _handlers.insert(ReqId::ID_REG_USER, [this](QJsonObject jsonObj){
        int error = jsonObj["error"].toInt();
        if (error != ErrorCodes::SUCCESS) {
            showErr(tr("参数错误"), false);
            qDebug() << "error is" << error;
            return;
        }

        auto email = jsonObj["email"].toString();
        showErr(tr("用户注册成功"), true);
        qDebug()<< "email is " << email;
        qDebug()<< "user uuid is " << jsonObj["uid"].toInt();
    });
}

void RegisterWidget::addTipMsg(TipMsg tm, QString tip)
{
    _tipMsgs[tm] = tip;
    showErr(tip, false);
}

void RegisterWidget::delTipMsg(TipMsg tm)
{
    _tipMsgs.remove(tm);
    if(_tipMsgs.empty()){
        ui->err_label->clear();
        return;
    }

    showErr(_tipMsgs.first(), false);
}

bool RegisterWidget::checkEmailValid()
{
    // 验证邮箱的地址正则表达式
    auto email = ui->email_LineEdit->text();
    // 邮箱的正则表达式
    QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
    bool match = regex.match(email).hasMatch(); // 执行正则表达式匹配
    if(!match){
        // 提示邮箱不正确
        addTipMsg(TipMsg::TIP_EMAIL_ERR, tr("邮箱地址不正确"));
        return false;
    }

    delTipMsg(TipMsg::TIP_EMAIL_ERR);
    return true;
}

bool RegisterWidget::checkPassValid()
{
    auto pass = ui->password_LineEdit->text();

    if(pass.length() < 6 || pass.length()>15){
        //提示长度不准确
        addTipMsg(TipMsg::TIP_PWD_ERR, tr("密码长度应为6~15"));
        return false;
    }

    // 创建一个正则表达式对象，按照下述密码要求
    // 这个正则表达式解释：
    // ^[a-zA-Z0-9!@#$%^&*]{6,15}$ 密码长度至少6，可以是字母、数字和特定的特殊字符
    QRegularExpression regExp("^[a-zA-Z0-9!@#$%^&*]{6,15}$");
    bool match = regExp.match(pass).hasMatch();
    if(!match){
        // 提示字符非法
        addTipMsg(TipMsg::TIP_PWD_ERR, tr("不能包含非法字符"));
        return false;
    }

    delTipMsg(TipMsg::TIP_PWD_ERR);
    return true;
}

bool RegisterWidget::checkVerifyValid()
{
    auto pass = ui->verify_LineEdit->text();
    if(pass.isEmpty()){
        addTipMsg(TipMsg::TIP_VERIFY_ERR, tr("验证码不能为空"));
        return false;
    }
    delTipMsg(TipMsg::TIP_VERIFY_ERR);
    return true;
}

void RegisterWidget::on_register_Button_clicked()
{
    bool valid = checkEmailValid();
    if(!valid){
        return;
    }
    valid = checkPassValid();
    if(!valid){
        return;
    }
    valid = checkVerifyValid();
    if(!valid){
        return;
    }

    // 发送 http post 请求注册用户
    QJsonObject jsonObj;
    jsonObj["email"] = ui->email_LineEdit->text();
    jsonObj["password"] = ui->password_LineEdit->text();
    jsonObj["verifycode"] = ui->verify_LineEdit->text();

    HttpMgr::getInstance()->postHttpReq(QUrl(gate_url_prefix + "/user_register"),
                                        jsonObj, ReqId::ID_REG_USER, Modules::REGISTERMOD);
}

void RegisterWidget::slot_reg_mod_finish(ReqId id, QString res, ErrorCodes err)
{
    if (err != ErrorCodes::SUCCESS) {
        showErr(tr("网络请求错误"), false);
        return;
    }

    // 解析 JSON 字符串,res需转化为QByteArray
    QJsonDocument jsonDoc = QJsonDocument::fromJson(res.toUtf8());

    // Json解析错误
    if (!jsonDoc.isObject()) {
        showErr(tr("json解析错误"), false);
        return;
    }

    QJsonObject jsonObj = jsonDoc.object();

    // 调用对应逻辑
    _handlers[id](jsonObj);
}


void RegisterWidget::on_getVerify_Button_clicked()
{
    auto email = ui->email_LineEdit->text();
    // 邮箱的正则表达式
    QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
    bool match = regex.match(email).hasMatch(); // 执行正则表达式匹配
    if (match){
        // 发送 http post 请求，获取验证码
        QJsonObject json_obj;
        json_obj["email"] = email;

        // 序列化
        HttpMgr::getInstance()->postHttpReq(QUrl(gate_url_prefix + "/get_varifycode"),
                                            json_obj,
                                            ReqId::ID_GET_VARIFY_CODE,
                                            Modules::REGISTERMOD);
    }
    else{
        //提示邮箱不正确
        showErr(tr("邮箱不正确"), false);
    }
}


void RegisterWidget::on_cancel_Button_clicked()
{
    emit sig_cancel();
}

