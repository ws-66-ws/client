#include "textbubble.h"

#include <QTextBlock>
#include <QFontMetricsF>

TextBubble::TextBubble(ChatRole role, const QString &text, QWidget *parent)
    : BubbleBase{role, parent}
{
    _textEdit = new QTextEdit();
    _textEdit->setReadOnly(true);
    _textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _textEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _textEdit->installEventFilter(this);
    QFont font("Microsoft YaHei");
    font.setPointSize(12);
    _textEdit->setFont(font);
    setPlainText(text);
    setWidget(_textEdit);
    initStyleSheet();
}

void TextBubble::setPlainText(const QString &text)
{
    _textEdit->setPlainText(text);
    //m_pTextEdit->setHtml(text);
    //找到段落中最大宽度
    qreal doc_margin = _textEdit->document()->documentMargin();
    int margin_left = this->layout()->contentsMargins().left();
    int margin_right = this->layout()->contentsMargins().right();
    QFontMetricsF fm(_textEdit->font());
    QTextDocument *doc = _textEdit->document();
    int max_width = 0;
    //遍历每一段找到 最宽的那一段
    for (QTextBlock it = doc->begin(); it != doc->end(); it = it.next())    //字体总长
    {
        int txtW = int(fm.horizontalAdvance(it.text()));
        max_width = max_width < txtW ? txtW : max_width;                 //找到最长的那段
    }
    //设置这个气泡的最大宽度 只需要设置一次
    setMaximumWidth(max_width + doc_margin * 2 + (margin_left + margin_right));        //设置最大宽度
}

void TextBubble::initStyleSheet()
{
    _textEdit->setStyleSheet("QTextEdit{background:transparent;border:none}");
}
