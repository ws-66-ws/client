#ifndef USERMGR_H
#define USERMGR_H

#include <QObject>
#include "Singleton.h"
#include "userdata.h"
#include <memory>
#include <QMap>


class UserMgr : public QObject, public Singleton<UserMgr>,
                public std::enable_shared_from_this<UserMgr>
{
    Q_OBJECT
public:
    friend class Singleton<UserMgr>;

    ~UserMgr();

    void setToken(QString token);

    void setUserInfo(std::shared_ptr<UserInfo> userInfo);

    int getUid();
    QString getNick();
    QString getDesc();
    QString getIcon();
    std::shared_ptr<UserInfo> getUserInfo();

    void appendFriendList(QJsonArray array);

    std::vector<std::shared_ptr<UserInfo>> getFriendList();

    std::shared_ptr<UserInfo> GetFriendById(int uid);

    int GetLastChatThreadId();
    void SetLastChatThreadId(int id);

    void AddChatThreadData(std::shared_ptr<ChatThreadData> chat_thread_data, int other_uid);

    int GetThreadIdByUid(int uid);

    std::shared_ptr<ChatThreadData> GetChatThreadByThreadId(int thread_id);

    std::shared_ptr<ChatThreadData> GetChatThreadByUid(int uid);
private:
    UserMgr();

private:
    std::shared_ptr<UserInfo> _userInfo;

    std::vector<std::shared_ptr<UserInfo>> _friendList;
    QMap<int, std::shared_ptr<UserInfo>> _friendMap;

    QString _token;

    int _chat_loaded;
    int _contact_loaded;
    //建立会话id到数据的映射关系
    QMap<int, std::shared_ptr<ChatThreadData>> _chat_map;
    //聊天会话id列表
    std::vector<int> _chat_thread_ids;
    //记录已经加载聊天列表的会话索引
    int _cur_load_chat_index;
    //上次会话的id
    int _last_chat_thread_id;
    //缓存其他用户uid和聊天的thread_id的映射关系。
    QMap<int, int> _uid_to_thread_id;
};

#endif // USERMGR_H
