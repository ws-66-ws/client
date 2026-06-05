#ifndef TEXTBUBBLE_H
#define TEXTBUBBLE_H

#include <QTextEdit>
#include <QWidget>
#include "global.h"
#include "bubblebase.h"

class TextBubble : public BubbleBase
{
    Q_OBJECT
public:
    explicit TextBubble(ChatRole role, const QString &text, QWidget *parent = nullptr);

private:
    void setPlainText(const QString &text);

    void initStyleSheet();
private:
    QTextEdit *_textEdit;
};

#endif // TEXTBUBBLE_H
