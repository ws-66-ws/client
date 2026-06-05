#ifndef TIMERBTN_H
#define TIMERBTN_H

#include <QPushButton>
#include <QTimer>
#include <QMouseEvent>

class TimerBtn : public QPushButton
{
    Q_OBJECT
public:
    explicit TimerBtn(QWidget *parent = nullptr);
    ~TimerBtn();

    // 重写mouseReleaseEvent
    void mouseReleaseEvent(QMouseEvent *event) override;
signals:

private:
    QTimer *_timer;

    int _counter;
};

#endif // TIMERBTN_H
