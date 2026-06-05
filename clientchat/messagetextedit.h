#ifndef MESSAGETEXTEDIT_H
#define MESSAGETEXTEDIT_H

#include <QTextEdit>
#include "global.h"
#include <QMouseEvent>
#include <QMimeData>

class MessageTextEdit : public QTextEdit
{
    Q_OBJECT
public:
    explicit MessageTextEdit(QWidget *parent = nullptr);

    ~MessageTextEdit();

    //提取消息列表
    QVector<MsgInfo> getMsgList();

    void insertFileFromUrl(const QStringList &urls);

signals:
    void send();

protected:
    //当鼠标拖着文件进入输入框时，它会允许进入，以便接收文件
    void dragEnterEvent(QDragEnterEvent *event);
    //当文件被松开时，它会调用 insertFromMimeData 来处理
    void dropEvent(QDropEvent *event);
    //发送快捷键
    void keyPressEvent(QKeyEvent *e);

private:
    //把图片进行等比例缩放
    void insertImages(const QString &url);
    //文件处理
    void insertTextFile(const QString &url);
    bool canInsertFromMimeData(const QMimeData *source) const;
    //会解析文件路径，区分是图片还是普通文件
    void insertFromMimeData(const QMimeData *source);

private:
    bool isImage(QString url);//判断文件是否为图片
    void insertMsgList(QVector<MsgInfo> &list, QString flag, QString text, QPixmap pix);

    QStringList getUrl(QString text);
    QPixmap getFileIconPixmap(const QString &url);//获取文件图标及大小信息，并转化成图片
    QString getFileSize(qint64 size);//获取文件大小

private slots:
    void textEditChanged();

private:
    QVector<MsgInfo> mMsgList;
    QVector<MsgInfo> mGetMsgList;
};

#endif // MESSAGETEXTEDIT_H
