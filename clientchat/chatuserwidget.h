#ifndef CHATUSERWIDGET_H
#define CHATUSERWIDGET_H

#include <QWidget>
#include "listitembase.h"
#include "userdata.h"

namespace Ui {
class ChatUserWidget;
}

class ChatUserWidget : public ListItemBase
{
    Q_OBJECT

public:
    explicit ChatUserWidget(QWidget *parent = nullptr);
    ~ChatUserWidget();

    void SetChatData(std::shared_ptr<ChatThreadData> chat_data);

    std::shared_ptr<ChatThreadData> GetChatData();

    void updateLastMsg(std::vector<std::shared_ptr<TextChatData>> msgs);

private:
    Ui::ChatUserWidget *ui;

    std::shared_ptr<ChatThreadData> _chat_data;
};

#endif // CHATUSERWIDGET_H
