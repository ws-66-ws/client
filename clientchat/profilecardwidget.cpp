#include "profilecardwidget.h"
#include "ui_profilecardwidget.h"
#include <QPixmap>

ProfileCardWidget::ProfileCardWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ProfileCardWidget)
{
    ui->setupUi(this);
}

ProfileCardWidget::~ProfileCardWidget()
{
    delete ui;
}

void ProfileCardWidget::SetInfo(std::shared_ptr<UserInfo> friendInfo)
{
    if(friendInfo == nullptr){
        return;
    }
    _friendInfo = friendInfo;

    // 设置头像
    QPixmap pixmap(friendInfo->_icon);
    if (!pixmap.isNull()) {
        ui->avatar_label->setPixmap(pixmap.scaled(ui->avatar_label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        ui->avatar_label->setScaledContents(true);
    }

    // 设置昵称
    ui->name_label->setText(friendInfo->_nick);

    // 设置email
    ui->email_label->setText("email: " + friendInfo->_email);

    // 设置性别
    if (friendInfo->_sex == 1) {
        QPixmap icon(":/resource/male_16.svg");
        ui->gender_icon->setPixmap(icon.scaled(16, 16, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        ui->gender_label->setText("男");
    } else if (friendInfo->_sex == 2) {
        QPixmap icon(":/resource/female_16.svg");
        ui->gender_icon->setPixmap(icon.scaled(16, 16, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        ui->gender_label->setText("女");
    }

    // 设置签名
    QPixmap signature(":/resource/edit_24.svg");
    ui->signature_icon->setPixmap(signature.scaled(16, 16, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->signature_label->setText(friendInfo->_desc);
}

std::shared_ptr<UserInfo> ProfileCardWidget::GetUserInfo()
{
    return _friendInfo;
}

void ProfileCardWidget::on_message_btn_clicked()
{
    emit sig_message_btn_clicked(_friendInfo);
}

