#include "chatwidget.h"
#include "ui_chatwidget.h"
#include <QRandomGenerator>
#include <QScrollBar>
#include <QTimer>
#include "usermgr.h"
#include "profilecardwidget.h"
#include <QJsonObject>
#include <QJsonArray>
#include "tcpmgr.h"
#include "textbubble.h"

ChatWidget::ChatWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ChatWidget)
    , _chatWdMod(ChatUIMode::ChatMode), _cur_chat_thread_id(0), _curConItem(nullptr)
    , _cur_chat_data(nullptr)
{
    ui->setupUi(this);

    // 加载自己的头像
    // _selfIconPath = ":/resource/head/head1.png";// 用于测试的路径
    _selfIconPath = UserMgr::getInstance()->getIcon();
    QPixmap selfIcon(_selfIconPath);
    ui->avatar_Label->setPixmap(selfIcon.scaled(ui->avatar_Label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->avatar_Label->setScaledContents(true);

    // 为搜索框添加 搜索action
    QAction *searchAction = new QAction(ui->search_Edit);
    searchAction->setIcon(QIcon(":/resource/search_24.svg"));
    ui->search_Edit->addAction(searchAction, QLineEdit::LeadingPosition);
    searchAction->setEnabled(false);

    // 为搜索框添加 清除action
    QAction *clearAction = new QAction(ui->search_Edit);
    clearAction->setIcon(QIcon(""));
    ui->search_Edit->addAction(clearAction, QLineEdit::TrailingPosition);

    // 当需要显示清除图标时，更改为实际的清除图标
    connect(ui->search_Edit, &QLineEdit::textChanged, [clearAction](const QString &text) {
        if (!text.isEmpty()) {
            clearAction->setIcon(QIcon(":/resource/close_circle_24.svg"));
        } else {
            clearAction->setIcon(QIcon("")); // 文本为空时，切换回透明图标
        }
    });

    // 连接清除动作的触发信号到槽函数，用于清除文本
    connect(clearAction, &QAction::triggered, [this, clearAction]() {
        ui->search_Edit->clear();
        clearAction->setIcon(QIcon("")); // 清除文本后，切换回透明图标
        ui->search_Edit->clearFocus();
        //清除按钮被按下则不显示搜索框
        //ShowSearch(false);
    });

    connect(ui->chatUser_List, &QListWidget::itemClicked,
            this, &ChatWidget::slot_chatUser_selected);

    connect(ui->contact_List, &QListWidget::itemClicked,
            this, &ChatWidget::slot_contactUser_selected);

    // 聊天布局滚动条滚动到最底部
    QScrollBar *bar = ui->scrollArea->verticalScrollBar();
    connect(bar, &QScrollBar::rangeChanged, this, &ChatWidget::onVScrollBarMoved);

    // 监听消息输入框文本变化
    connect(ui->message_Edit, &QTextEdit::textChanged, this, [this]() {
        // 如果有内容则启用发送按钮，否则禁用
        ui->send_Button->setEnabled(!ui->message_Edit->toPlainText().isEmpty());
    });

    connect(ui->conInfo, &ProfileCardWidget::sig_message_btn_clicked, this, &ChatWidget::slot_message_btn_clicked);

    //连接对端消息通知
    connect(TcpMgr::getInstance().get(), &TcpMgr::sig_text_chat_msg,
            this, &ChatWidget::slot_text_chat_msg);

    // 用于心跳检查
    _timer = new QTimer(this);
    connect(_timer, &QTimer::timeout, this, [this]() {
        auto userInfo = UserMgr::getInstance()->getUserInfo();
        QJsonObject textObj;
        textObj["fromuid"] = userInfo->_uid;
        QJsonDocument doc(textObj);
        QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
        emit TcpMgr::getInstance()->sig_send_data(ReqId::ID_HEART_BEAT_REQ, jsonData);
    });
    _timer->start(10000);

    //连接tcp返回的创建私聊的回复
    connect(TcpMgr::getInstance().get(), &TcpMgr::sig_create_private_chat,
            this, &ChatWidget::slot_create_private_chat);
}

ChatWidget::~ChatWidget()
{
    _timer->stop();
    delete ui;
}

void ChatWidget::loadChatList()
{

    // 加载联系人
    loadContactList();
}

void ChatWidget::loadContactList()
{
    // 获取 UserMgr 单例引用
    const auto &friendList = UserMgr::getInstance()->getFriendList();

    if(friendList.empty()){
        return;
    }

    // 清空联系人列表的所有旧项
    ui->contact_List->clear();

    // 遍历好友列表，为每个好友创建列表项和对应控件
    for (const auto &friendInfoPtr : friendList) {
        if (!friendInfoPtr) {
            continue;   // 跳过空指针
        }

        // 创建列表项
        ConUserItem *widget = new ConUserItem();
        widget->SetInfo(friendInfoPtr);
        QListWidgetItem *item = new QListWidgetItem();

        item->setSizeHint(widget->sizeHint());

        // 将自定义控件绑定到列表项上
        ui->contact_List->addItem(item);
        ui->contact_List->setItemWidget(item, widget);
    }
}

void ChatWidget::setChatData(std::shared_ptr<ChatThreadData> chat_data)
{
    _cur_chat_data = chat_data;
    auto other_id = _cur_chat_data->GetOtherId();
    if(other_id == 0) {
        //说明是群聊
        ui->currentChat_Label->setText(_cur_chat_data->GetGroupName());
        //todo...加载群聊信息和成员信息
        return;
    }

    //私聊
    auto friend_info = UserMgr::getInstance()->GetFriendById(other_id);
    if (friend_info == nullptr) {
        return;
    }
    ui->currentChat_Label->setText(friend_info->_nick);
    removeAllItem();
    _unrsp_item_map.clear();
    for(auto & msg : chat_data->GetMsgMapRef()){
        AppendChatMsg(msg);
    }

    for (auto& msg : chat_data->GetMsgUnRspRef()) {
        AppendChatMsg(msg);
    }
}

void ChatWidget::AppendChatMsg(std::shared_ptr<ChatDataBase> msg)
{
    auto self_info = UserMgr::getInstance()->getUserInfo();
    ChatRole role;
    if (msg->GetSendUid() == self_info->_uid) {
        role = ChatRole::Self;
        ChatMessageWidget* pChatItem = new ChatMessageWidget(role);

        pChatItem->setIcon(QPixmap(self_info->_icon));
        QWidget* pBubble = nullptr;
        if (msg->GetMsgType() == ChatMsgType::TEXT) {
            pBubble = new TextBubble(role, msg->GetMsgContent());
        }

        pChatItem->setWidget(pBubble);
        // auto status = msg->GetStatus();
        // pChatItem->setStatus(status);
        ui->layout->insertWidget(ui->layout->count() - 1, pBubble);
        // if (status == 0) {
        //     _unrsp_item_map[msg->GetUniqueId()] = pChatItem;
        // }
    }
    else {
        role = ChatRole::Other;
        ChatMessageWidget* pChatItem = new ChatMessageWidget(role);
        auto friend_info = UserMgr::getInstance()->GetFriendById(msg->GetSendUid());
        if (friend_info == nullptr) {
            return;
        }
        pChatItem->setIcon(QPixmap(friend_info->_icon));
        QWidget* pBubble = nullptr;
        if (msg->GetMsgType() == ChatMsgType::TEXT) {
            pBubble = new TextBubble(role, msg->GetMsgContent());
        }
        pChatItem->setWidget(pBubble);
        // auto status = msg->GetStatus();
        // pChatItem->setStatus(status);
        ui->layout->insertWidget(ui->layout->count() - 1, pBubble);
        // if (status == 0) {
        //     _unrsp_item_map[msg->GetUniqueId()] = pChatItem;
        // }
    }
}

void ChatWidget::on_contact_Button_clicked()
{
    if(_chatWdMod != ContactMode){
        // 最左侧侧边栏 切换到联系人页面 (索引为 1)
        ui->stackedWidget->setCurrentIndex(1);

        // 切换 最右侧stackedContent 切换
        if(_curConItem != nullptr){
            ui->stackedContent->setCurrentIndex(2);
        }
        else{
            ui->stackedContent->setCurrentIndex(0);
        }

        // 更换图标状态
        // 消息图标变灰/未激活
        ui->message_Button->setIcon(QIcon(":/resource/nav_message_normal_24.svg"));
        // 联系人图标变亮/激活
        ui->contact_Button->setIcon(QIcon(":/resource/nav_contact_active_24.svg"));

        //更新当前模式状态，方便后续逻辑判断
        _chatWdMod = ContactMode;
    }
}


void ChatWidget::on_message_Button_clicked()
{
    if(_chatWdMod != ChatMode){
        // 最左侧侧边栏 切换回默认聊天用户列表 (索引为 0)
        ui->stackedWidget->setCurrentIndex(0);

        // 切换 最右侧stackedContent 切换
        if(_cur_chat_thread_id != 0){
            ui->stackedContent->setCurrentIndex(1);
        }
        else{
            ui->stackedContent->setCurrentIndex(0);
        }

        // 更换图标状态
        // 消息图标变亮/激活
        ui->message_Button->setIcon(QIcon(":/resource/nav_message_active_24.svg"));
        // 联系人图标变灰/未激活
        ui->contact_Button->setIcon(QIcon(":/resource/nav_contact_normal_24.svg"));

        //更新当前模式状态，方便后续逻辑判断
        _chatWdMod = ChatMode;
    }
}

void ChatWidget::slot_chatUser_selected(QListWidgetItem *item)
{
    // 获取被点击项对应的自定义 Widget
    ChatUserWidget *widget = qobject_cast<ChatUserWidget*>(ui->chatUser_List->itemWidget(item));
    if (!widget)
        return;

    auto chat_data = widget->GetChatData();
    _cur_chat_data = chat_data;

    // 判断当前是否在聊天界面 (stackedContent 的第 1 页是聊天页)
    bool isChatPage = (ui->stackedContent->currentIndex() == 1);

    if (!isChatPage) {
        // 切换到聊天页 (index 1)
        ui->stackedContent->setCurrentIndex(1);

        // 设置聊天对象
        setChatData(chat_data);

        // 记录当前选中Item的chat_thread_id
        _cur_chat_thread_id = widget->GetChatData()->GetThreadId();
    }
    else {
        if (_cur_chat_thread_id == widget->GetChatData()->GetThreadId()) {
            // 点击了相同的 item -> 关闭聊天界面

            // 切换到默认主页 (index 0)
            ui->stackedContent->setCurrentIndex(0);
            // 清空记录
            _cur_chat_thread_id = 0;
            // 取消该列表项的选中高亮状态
            item->setSelected(false);

        } else {
            // 点击了不同的 item -> 切换聊天对象

            // 设置相应的聊天对象
            setChatData(chat_data);

            // 更新记录
            _cur_chat_thread_id = widget->GetChatData()->GetThreadId();

        }
    }
}


void ChatWidget::slot_contactUser_selected(QListWidgetItem *item)
{
    ConUserItem *widget = qobject_cast<ConUserItem*>(ui->contact_List->itemWidget(item));
    if (!widget)
        return;

    ProfileCardWidget *card = qobject_cast<ProfileCardWidget*>(ui->conInfo);
    if (!card)
        return;

    // 判断当前是否在联系人资料界面 (stackedContent 的第 2 页是联系人资料页)
    bool isContactInfoPage = (ui->stackedContent->currentIndex() == 2);

    if (!isContactInfoPage) {
        // 切换到聊天页 (index 2)
        ui->stackedContent->setCurrentIndex(2);
        // 设置资料界面
        card->SetInfo(widget->GetUserInfo());

        // 记录当前选中的Item
        _curConItem = widget;

    }
    else {
        if (_curConItem == widget) {
            // 点击了相同的 item -> 关闭聊天界面

            // 切换到默认主页 (index 0)
            ui->stackedContent->setCurrentIndex(0);
            // 清空记录
            _curConItem = nullptr;
            // 取消该列表项的选中高亮状态
            item->setSelected(false);

        } else {
            // 点击了不同的 item -> 切换聊天对象

            // 设置资料界面
            card->SetInfo(widget->GetUserInfo());
            // 更新记录
            _curConItem = widget;

        }
    }


}


void ChatWidget::on_send_Button_clicked()
{
    if (_cur_chat_data == nullptr || _cur_chat_thread_id == 0) {
        qDebug() << "friend_info is empty";
        return;
    }

    auto user_info = UserMgr::getInstance()->getUserInfo();
    auto pTextEdit = ui->message_Edit;
    ChatRole role = ChatRole::Self;
    QString userName = user_info->_nick;
    QString userIcon = user_info->_icon;

    const QVector<MsgInfo>& msgList = pTextEdit->getMsgList();
    QJsonObject textObj;
    QJsonArray textArray;
    int txt_size = 0;
    auto thread_id = _cur_chat_data->GetThreadId();
    for(int i = 0; i < msgList.size(); ++i)
    {
        //消息内容长度不合规就跳过
        if(msgList[i].content.length() > 1024){
            continue;
        }

        QString type = msgList[i].msgFlag;
        ChatMessageWidget *pChatItem = new ChatMessageWidget(role);
        pChatItem->setIcon(QPixmap(userIcon));
        QWidget *pBubble = nullptr;
        //生成唯一id
        QUuid uuid = QUuid::createUuid();
        //转为字符串
        QString uuidString = uuid.toString();
        if(type == "text")
        {
            pBubble = new TextBubble(role, msgList[i].content);
            if(txt_size + msgList[i].content.length() > 1024){
                textObj["fromuid"] = user_info->_uid;
                textObj["touid"] = _cur_chat_data->GetOtherId();
                textObj["thread_id"] = thread_id;
                textObj["text_array"] = textArray;
                QJsonDocument doc(textObj);
                QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
                //发送并清空之前累计的文本列表
                txt_size = 0;
                textArray = QJsonArray();
                textObj = QJsonObject();
                //发送tcp请求给chat server
                emit TcpMgr::getInstance()->sig_send_data(ReqId::ID_TEXT_CHAT_MSG_REQ, jsonData);
            }

            //将bubble和uid绑定，以后可以等网络返回消息后设置是否送达
            //_bubble_map[uuidString] = pBubble;
            txt_size += msgList[i].content.length();
            QJsonObject obj;
            QByteArray utf8Message = msgList[i].content.toUtf8();
            auto content = QString::fromUtf8(utf8Message);
            obj["content"] = content;
            obj["unique_id"] = uuidString;
            textArray.append(obj);
            //todo... 注意，此处先按私聊处理
            auto txt_msg = std::make_shared<TextChatData>(uuidString, thread_id, ChatFormType::PRIVATE,
                                                          ChatMsgType::TEXT, content, user_info->_uid, 0);
            //将未回复的消息加入到未回复列表中，以便后续处理
            _cur_chat_data->AppendUnRspMsg(uuidString, txt_msg);
        }
        // else if(type == "image")
        // {
        //     pBubble = new PictureBubble(QPixmap(msgList[i].content) , role);
        // }
        // else if(type == "file")
        // {

        // }
        //发送消息
        if(pBubble != nullptr)
        {
            pChatItem->setWidget(pBubble);
            ui->layout->insertWidget(ui->layout->count() - 1, pBubble);
            _unrsp_item_map[uuidString] = pChatItem;
        }

    }

    qDebug() << "textArray is " << textArray ;
    //发送给服务器
    textObj["text_array"] = textArray;
    textObj["fromuid"] = user_info->_uid;
    textObj["touid"] = _cur_chat_data->GetOtherId();
    textObj["thread_id"] = thread_id;
    QJsonDocument doc(textObj);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
    //发送并清空之前累计的文本列表
    txt_size = 0;
    textArray = QJsonArray();
    textObj = QJsonObject();
    //发送tcp请求给chat server
    // emit TcpMgr::getInstance()->sig_send_data(ReqId::ID_TEXT_CHAT_MSG_REQ, jsonData);
}

void ChatWidget::onVScrollBarMoved(int min, int max)
{
    QScrollBar *bar = ui->scrollArea->verticalScrollBar();
    bar->setValue(bar->maximum());
}

void ChatWidget::slot_message_btn_clicked(std::shared_ptr<UserInfo> friendInfo)
{
    if (friendInfo == nullptr) {
        return;   // 跳过空指针
    }

    auto other_uid = friendInfo->_uid;

    // 尝试从 UserMgr 获取现有的聊天会话数据
    auto chat_thread_data = UserMgr::getInstance()->GetChatThreadByUid(other_uid);
    //如果找到
    if (chat_thread_data) {
        auto find_iter = _chat_thread_items.find(chat_thread_data->GetThreadId());
        if (find_iter != _chat_thread_items.end()) {
            qDebug() << "jump to chat item , friend uid is " << friendInfo->_uid;
            ui->chatUser_List->scrollToItem(find_iter.value());

        } //说明之前有缓存过聊天列表，只是被删除了，那么重新加进来即可
        else {
            // 创建列表项
            auto* chat_user_wid = new ChatUserWidget();
            chat_user_wid->SetChatData(chat_thread_data);
            QListWidgetItem* item = new QListWidgetItem;
            item->setSizeHint(chat_user_wid->sizeHint());

            // 将自定义控件绑定到列表项上
            ui->chatUser_List->insertItem(0, item);
            ui->chatUser_List->setItemWidget(item, chat_user_wid);

            _chat_thread_items.insert(chat_thread_data->GetThreadId(), item);

        }
        // 最左侧侧边栏 切换回默认聊天用户列表 (索引为 0)
        ui->stackedWidget->setCurrentIndex(0);
        // 更换图标状态
        // 消息图标变亮/激活
        ui->message_Button->setIcon(QIcon(":/resource/nav_message_active_24.svg"));
        // 联系人图标变灰/未激活
        ui->contact_Button->setIcon(QIcon(":/resource/nav_contact_normal_24.svg"));
        //更新当前模式状态，方便后续逻辑判断
        _chatWdMod = ChatMode;

        // 记录当前选中的Item的thread_id
        _cur_chat_thread_id = chat_thread_data->GetThreadId();
        // 设置该列表项的选中高亮状态
        _chat_thread_items[chat_thread_data->GetThreadId()]->setSelected(true);

        // 切换 最右侧stackedContent
        ui->stackedContent->setCurrentIndex(1);
        // 调用 setChatData 加载历史消息
        setChatData(chat_thread_data);

        _cur_chat_data = chat_thread_data; // 方便后续发送消息时使用
        return;
    }

    //如果没找到，则发送创建请求
    auto uid = UserMgr::getInstance()->getUid();
    QJsonObject jsonObj;
    jsonObj["uid"] = uid;
    jsonObj["other_id"] = friendInfo->_uid;

    QJsonDocument doc(jsonObj);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);

    //发送tcp请求给chat server
    emit TcpMgr::getInstance()->sig_send_data(ReqId::ID_CREATE_PRIVATE_CHAT_REQ, jsonData);
}

void ChatWidget::slot_text_chat_msg(std::vector<std::shared_ptr<TextChatData> > msglists)
{

}

void ChatWidget::slot_create_private_chat(int uid, int other_id, int thread_id)
{
    auto* chat_user_wid = new ChatUserWidget();
    auto chat_thread_data = std::make_shared<ChatThreadData>(other_id, thread_id, 0);
    if (chat_thread_data == nullptr) {
        return;
    }

    UserMgr::getInstance()->AddChatThreadData(chat_thread_data, other_id);

    // 设置item的GUI
    chat_user_wid->SetChatData(chat_thread_data);

    QListWidgetItem* item = new QListWidgetItem;
    item->setSizeHint(chat_user_wid->sizeHint());
    qDebug() << "chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
    ui->chatUser_List->insertItem(0, item);
    ui->chatUser_List->setItemWidget(item, chat_user_wid);
    _chat_thread_items.insert(thread_id, item);


    // 最左侧侧边栏 切换回默认聊天用户列表 (索引为 0)
    ui->stackedWidget->setCurrentIndex(0);
    // 更换图标状态
    // 消息图标变亮/激活
    ui->message_Button->setIcon(QIcon(":/resource/nav_message_active_24.svg"));
    // 联系人图标变灰/未激活
    ui->contact_Button->setIcon(QIcon(":/resource/nav_contact_normal_24.svg"));
    //更新当前模式状态，方便后续逻辑判断
    _chatWdMod = ChatMode;

    // 记录当前选中的Item的thread_id
    _cur_chat_thread_id = chat_thread_data->GetThreadId();
    // 设置该列表项的选中高亮状态
    _chat_thread_items[chat_thread_data->GetThreadId()]->setSelected(true);

    // 切换 最右侧stackedContent
    ui->stackedContent->setCurrentIndex(1);
    // 调用 setChatData 加载历史消息
    setChatData(chat_thread_data);

    _cur_chat_data = chat_thread_data; // 方便后续发送消息时使用

    return;
}

void ChatWidget::SetSelectChatItem(int thread_id)
{
    if (ui->chatUser_List->count() <= 0) {
        return;
    }

    if (thread_id == 0) {
        ui->chatUser_List->setCurrentRow(0);
        QListWidgetItem* firstItem = ui->chatUser_List->item(0);
        if (!firstItem) {
            return;
        }

        //转为widget
        QWidget* widget = ui->chatUser_List->itemWidget(firstItem);
        if (!widget) {
            return;
        }

        auto con_item = qobject_cast<ChatUserWidget*>(widget);
        if (!con_item) {
            return;
        }

        _cur_chat_thread_id = con_item->GetChatData()->GetThreadId();

        return;
    }

    auto find_iter = _chat_thread_items.find(thread_id);
    if (find_iter == _chat_thread_items.end()) {
        qDebug() << "thread_id [" << thread_id << "] not found, set curent row 0";
        ui->chatUser_List->setCurrentRow(0);
        return;
    }

    ui->chatUser_List->setCurrentItem(find_iter.value());

    _cur_chat_thread_id = thread_id;
}

void ChatWidget::SetSelectChatPage(int thread_id)
{
    if (ui->chatUser_List->count() <= 0) {
        return;
    }

    if (thread_id == 0) {
        auto item = ui->chatUser_List->item(0);
        //转为widget
        QWidget* widget = ui->chatUser_List->itemWidget(item);
        if (!widget) {
            return;
        }

        auto con_item = qobject_cast<ChatUserWidget*>(widget);
        if (!con_item) {
            return;
        }

        //设置信息
        auto chat_data = con_item->GetChatData();
        setChatData(chat_data);
        return;
    }

    auto find_iter = _chat_thread_items.find(thread_id);
    if (find_iter == _chat_thread_items.end()) {
        return;
    }

    //转为widget
    QWidget* widget = ui->chatUser_List->itemWidget(find_iter.value());
    if (!widget) {
        return;
    }

    //判断转化为自定义的widget
    // 对自定义widget进行操作， 将item 转化为基类ListItemBase
    ListItemBase* customItem = qobject_cast<ListItemBase*>(widget);
    if (!customItem) {
        qDebug() << "qobject_cast<ListItemBase*>(widget) is nullptr";
        return;
    }

    auto itemType = customItem->getItemType();
    if (itemType == CHAT_USER_ITEM) {
        auto con_item = qobject_cast<ChatUserWidget*>(customItem);
        if (!con_item) {
            return;
        }

        //设置信息
        auto chat_data = con_item->GetChatData();
        setChatData(chat_data);

        return;
    }
}

void ChatWidget::removeAllItem()
{
    int count = ui->layout->count();

    for (int i = 0; i < count - 1; ++i) {
        QLayoutItem *item = ui->layout->takeAt(0); // 始终从第一个控件开始删除
        if (item) {
            if (QWidget *widget = item->widget()) {
                delete widget;
            }
            delete item;
        }
    }
}


