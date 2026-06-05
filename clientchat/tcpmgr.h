#ifndef TCPMGR_H
#define TCPMGR_H

#include <QObject>
#include <QTcpSocket>
#include "Singleton.h"
#include "global.h"
#include "userdata.h"

class TcpMgr : public QObject,
               public Singleton<TcpMgr>,
               public std::enable_shared_from_this<TcpMgr>
{
    Q_OBJECT
public:
    TcpMgr();

    void closeConnection();

private:
    void initHandlers();

    void handlerMsg(ReqId id, int len, QByteArray data);

public slots:
    void slot_tcp_connect(ServerInfo);

    void slot_send_data(ReqId id, QString data);


signals:
    void sig_con_success(bool bsuccess);

    void sig_send_data(ReqId id, QString data);

    void sig_login_failed(int);

    void sig_swich_chatwd();

    void sig_notify_offline();

    void sig_connection_closed();

    void sig_text_chat_msg(std::vector<std::shared_ptr<TextChatData>> msg_list);

    void sig_create_private_chat(int uid, int other_id, int thread_id);

private:
    QTcpSocket _tcpSocket;

    QString _host;

    uint16_t _port;

    QByteArray _buffer;

    // 标记当前是否有未处理完的接收数据（用于处理粘包/半包）
    bool _bRecvPending;

    quint16 _messageId;

    quint16 _messageLen;

    QMap<ReqId, std::function<void(ReqId id, int len, QByteArray data)>> _handlers;
};

#endif // TCPMGR_H
