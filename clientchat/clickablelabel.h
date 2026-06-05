#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

#include <QLabel>
#include <QMouseEvent>

class ClickableLabel : public QLabel
{
    Q_OBJECT
public:
    explicit ClickableLabel(QWidget *parent = nullptr);

signals:
    void clicked();

private:
    void mouseReleaseEvent(QMouseEvent* event) override;
};

#endif // CLICKABLELABEL_H
