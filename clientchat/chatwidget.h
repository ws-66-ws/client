#ifndef CHATWIDGET_H
#define CHATWIDGET_H

#include <QWidget>
#include "global.h"
#include <QListWidget>
#include "chatuserwidget.h"
#include "conuseritem.h"
#include <QMap>
#include "chatmessagewidget.h"

namespace Ui {
class ChatWidget;
}

class ChatWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChatWidget(QWidget *parent = nullptr);
    ~ChatWidget();

    void loadChatList();

    void loadContactList();

    // 加载聊天信息
    void setChatData(std::shared_ptr<ChatThreadData> chat_data);

    void AppendChatMsg(std::shared_ptr<ChatDataBase> msg);

private slots:
    void on_contact_Button_clicked();

    void on_message_Button_clicked();

    // 处理点击列表项的信号槽
    void slot_chatUser_selected(QListWidgetItem *item);

    void slot_contactUser_selected(QListWidgetItem *item);

    void on_send_Button_clicked();
    // 移动滚动条
    void onVScrollBarMoved(int min, int max);

    void slot_message_btn_clicked(std::shared_ptr<UserInfo> friendInfo);

    void slot_text_chat_msg(std::vector<std::shared_ptr<TextChatData>> msglists);

    void slot_create_private_chat(int uid, int other_id, int thread_id);
private:
    void SetSelectChatItem(int thread_id);

    void SetSelectChatPage(int thread_id);

    void removeAllItem();

private:
    Ui::ChatWidget *ui;

    ChatUIMode _chatWdMod;
    //chat_thred_id和对应的item的映射关系。
    QMap<int, QListWidgetItem*>  _chat_thread_items;
    // 当前的chat_thread_id
    int _cur_chat_thread_id;
    // 当前联系人的item
    ConUserItem *_curConItem;

    QString _selfIconPath;

    QTimer *_timer;

    std::shared_ptr<ChatThreadData> _cur_chat_data;
    QMap<QString, QWidget*>  _bubble_map;
    QHash<QString, ChatMessageWidget*> _unrsp_item_map;
};

#endif // CHATWIDGET_H
