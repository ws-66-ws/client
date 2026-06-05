#ifndef PROFILECARDWIDGET_H
#define PROFILECARDWIDGET_H

#include <QWidget>
#include "userdata.h"

namespace Ui {
class ProfileCardWidget;
}

class ProfileCardWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ProfileCardWidget(QWidget *parent = nullptr);
    ~ProfileCardWidget();

    void SetInfo(std::shared_ptr<UserInfo> friendInfo);

    std::shared_ptr<UserInfo> GetUserInfo();

private slots:
    void on_message_btn_clicked();

signals:
    void sig_message_btn_clicked(std::shared_ptr<UserInfo> friendInfo);

private:
    Ui::ProfileCardWidget *ui;

    std::shared_ptr<UserInfo> _friendInfo;
};

#endif // PROFILECARDWIDGET_H
