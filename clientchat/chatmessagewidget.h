#ifndef CHATMESSAGEWIDGET_H
#define CHATMESSAGEWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include "global.h"

class ChatMessageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ChatMessageWidget(ChatRole role, QWidget *parent = nullptr);

    void setIcon(const QPixmap &icon);

    void setWidget(QWidget *w);
private:
    ChatRole _role;

    QLabel *_iconLabel;

    QWidget *_bubble;

};

#endif // CHATMESSAGEWIDGET_H
