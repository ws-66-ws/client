#ifndef HTTPMGR_H
#define HTTPMGR_H

#include <QNetworkAccessManager>
#include <QObject>
#include "Singleton.h"
#include <QString>
#include <QUrl>
#include <QJsonObject>
#include <QJsonDocument>
#include "global.h"
#include <memory>

class HttpMgr : public QObject, public Singleton<HttpMgr>
    , public std::enable_shared_from_this<HttpMgr>
{
    Q_OBJECT
public:
    ~HttpMgr();

    void postHttpReq(QUrl url, QJsonObject json, ReqId req_id, Modules mod);

private slots:
    void slot_http_finish(ReqId id, QString res, ErrorCodes err, Modules mod);

private:
    friend class Singleton<HttpMgr>;
    explicit HttpMgr(QObject *parent = nullptr);

    QNetworkAccessManager _manager;
signals:
    void sig_http_finish(ReqId id, QString res, ErrorCodes err, Modules mod);

    void sig_reg_mod_finish(ReqId id, QString res, ErrorCodes err);

    void sig_login_mod_finish(ReqId id, QString res, ErrorCodes err);
};

#endif // HTTPMGR_H
