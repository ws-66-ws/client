#include "conuseritem.h"
#include "ui_conuseritem.h"

ConUserItem::ConUserItem(QWidget *parent)
    : ListItemBase(parent)
    , ui(new Ui::ConUserItem), _friendInfo(nullptr)
{
    ui->setupUi(this);

    setItemType(ListItemType::CONTACT_USER_ITEM);
}

ConUserItem::~ConUserItem()
{
    delete ui;
}

void ConUserItem::SetInfo(std::shared_ptr<UserInfo> friendInfo)
{
    if(friendInfo == nullptr){
        return;
    }
    _friendInfo = friendInfo;

    auto icon = _friendInfo->_icon;
    auto nick = _friendInfo->_nick;
    auto desc = _friendInfo->_desc;

    // 加载图片
    QPixmap pixmap(icon);
    // 设置图片自动缩放
    ui->icon_label->setPixmap(pixmap.scaled(ui->icon_label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_label->setScaledContents(true);

    int maxNameChars = 9; // 用户名只显示前9个字符
    int maxMsgChars = 20; // 消息只显示前20个字符

    if (nick.length() > maxNameChars) {
        // 截断并手动加上省略号
        QString shortText = nick.left(maxNameChars) + "...";
        ui->userName_label->setText(shortText);
    }
    else{
        ui->userName_label->setText(nick);
    }

    if (desc.length() > maxMsgChars) {
        // 截断并手动加上省略号
        QString shortText = desc.left(maxMsgChars) + "...";
        ui->state_label->setText(shortText);
    }
    else{
        ui->state_label->setText(desc);
    }
}

std::shared_ptr<UserInfo> ConUserItem::GetUserInfo()
{
    return _friendInfo;
}
