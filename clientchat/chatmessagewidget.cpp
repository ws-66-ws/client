#include "chatmessagewidget.h"
#include <QPainter>
#include <QPixmap>

ChatMessageWidget::ChatMessageWidget(ChatRole role, QWidget *parent)
    : QWidget{parent}
    , _role(role)
{
    QHBoxLayout *_mainLayout = new QHBoxLayout(this);
    _mainLayout->setContentsMargins(5, 5, 5, 5);
    _mainLayout->setSpacing(8);

    // 头像
    _iconLabel = new QLabel(this);
    _iconLabel->setFixedSize(36, 36);
    _iconLabel->setStyleSheet("border-radius: 18px; background-color: #555;");

    // 根据是否自己发送，设置不同的样式和布局方向
    if (_role == ChatRole::Self) {
        // 自己的消息：蓝色气泡，靠右显示
        _bubble->setStyleSheet(
            "background-color: #1976d2;"
            "color: white;"
            "border-radius: 12px;"
            "padding: 8px 14px;"
            "font-size: 14px;"
            "border-top-right-radius: 4px;"
            );
        _mainLayout->addStretch();                // 左侧弹簧
        _mainLayout->addWidget(_bubble);     // 气泡
        _mainLayout->addWidget(_iconLabel);      // 头像在右侧
        _mainLayout->setAlignment(_iconLabel, Qt::AlignTop);
    }
    else {
        // 对方消息：灰色气泡，靠左显示
        _bubble->setStyleSheet(
            "background-color: #3a3a3a;"
            "color: #e0e0e0;"
            "border-radius: 12px;"
            "padding: 8px 14px;"
            "font-size: 14px;"
            "border-top-left-radius: 4px;"
            );
        _mainLayout->addWidget(_iconLabel);      // 头像在左侧
        _mainLayout->addWidget(_bubble);     // 气泡
        _mainLayout->addStretch();                // 右侧弹簧
        _mainLayout->setAlignment(_iconLabel, Qt::AlignTop);
    }

}

void ChatMessageWidget::setIcon(const QPixmap &icon)
{
    _iconLabel->setPixmap(icon);
}

void ChatMessageWidget::setWidget(QWidget *w)
{
    QGridLayout *pGLayout = (qobject_cast<QGridLayout *>)(this->layout());
    pGLayout->replaceWidget(_bubble, w);
    delete _bubble;
    _bubble = w;
}
