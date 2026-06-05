#ifndef REGISTERWIDGET_H
#define REGISTERWIDGET_H

#include <QWidget>
#include "global.h"

namespace Ui {
class RegisterWidget;
}

class RegisterWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RegisterWidget(QWidget *parent = nullptr);
    ~RegisterWidget();

    void showErr(QString str, bool);

private:
    // 初始化处理 http 的函数
    void initHttpHandlers();

    // 添加信息
    void addTipMsg(TipMsg tm, QString tip);

    // 删除信息
    void delTipMsg(TipMsg tm);

    // 检查邮箱是否有效
    bool checkEmailValid();

    // 检查密码是否有效
    bool checkPassValid();

    // 检查验证码是否有效
    bool checkVerifyValid();

signals:
    // 取消信号
    void sig_cancel();

private slots:
    void on_register_Button_clicked();

    void slot_reg_mod_finish(ReqId id, QString res, ErrorCodes err);

    void on_getVerify_Button_clicked();

    void on_cancel_Button_clicked();

private:
    Ui::RegisterWidget *ui;

    QMap<ReqId, std::function<void(const QJsonObject &)>> _handlers;

    QMap<TipMsg, QString> _tipMsgs; // 用来缓存各个输入框输入完成后提示的信息
};

#endif // REGISTERWIDGET_H
