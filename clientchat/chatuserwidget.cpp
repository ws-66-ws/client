#include "chatuserwidget.h"
#include "ui_chatuserwidget.h"
#include "usermgr.h"

ChatUserWidget::ChatUserWidget(QWidget *parent)
    : ListItemBase(parent)
    , ui(new Ui::ChatUserWidget), _chat_data(nullptr)
{
    ui->setupUi(this);

    setItemType(ListItemType::CHAT_USER_ITEM);
}

ChatUserWidget::~ChatUserWidget()
{
    delete ui;
}

void ChatUserWidget::SetChatData(std::shared_ptr<ChatThreadData> chat_data)
{
    _chat_data = chat_data;
    auto other_id = _chat_data->GetOtherId();
    auto other_info = UserMgr::getInstance()->GetFriendById(other_id);
    // 加载图片
    QPixmap pixmap(other_info->_icon);

    // 设置图片自动缩放
    ui->icon_label->setPixmap(pixmap.scaled(ui->icon_label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_label->setScaledContents(true);

    // 昵称
    ui->userName_label->setText(other_info->_nick);

    // 最后一条消息
    ui->message_label->setText(chat_data->GetLastMsg());

    // 时间

    // 未读消息的数量
}

std::shared_ptr<ChatThreadData> ChatUserWidget::GetChatData()
{
    return _chat_data;
}

void ChatUserWidget::updateLastMsg(std::vector<std::shared_ptr<TextChatData>> msgs)
{
    int last_msg_id = 0;
    QString last_msg = "";
    for (auto& msg : msgs) {
        last_msg = msg->GetContent();
        last_msg_id = msg->GetMsgId();
        _chat_data->AddMsg(msg);
    }

    _chat_data->SetLastMsgId(last_msg_id);
    ui->message_label->setText(last_msg);
}

