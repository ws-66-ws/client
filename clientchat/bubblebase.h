#ifndef BUBBLEBASE_H
#define BUBBLEBASE_H

#include <QHBoxLayout>
#include <QWidget>
#include "global.h"


class BubbleBase : public QWidget
{
    Q_OBJECT
public:
    explicit BubbleBase(ChatRole role, QWidget *parent = nullptr);

    void setWidget(QWidget *w);
private:
    ChatRole _role;
    QHBoxLayout *_layout;   // 内部布局，用于对齐气泡的附加元素（如小三角）
};

#endif // BUBBLEBASE_H
