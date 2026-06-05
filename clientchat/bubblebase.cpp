#include "bubblebase.h"

BubbleBase::BubbleBase(ChatRole role, QWidget *parent)
    : QWidget{parent}
    , _role(role)
{
    _layout = new QHBoxLayout(this);
    _layout->setContentsMargins(0, 0, 0, 0);
}

void BubbleBase::setWidget(QWidget *w)
{
    if(_layout->count() > 0)
        return ;
    else{
        _layout->addWidget(w);
    }
}
