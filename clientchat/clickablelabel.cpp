#include "clickablelabel.h"

ClickableLabel::ClickableLabel(QWidget *parent)
    : QLabel{parent}
{}

void ClickableLabel::mouseReleaseEvent(QMouseEvent *event)
{
   if (event && event->button() == Qt::LeftButton) {
       emit clicked(); // 发出点击信号
   }
}
