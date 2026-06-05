#include "usermgr.h"
#include <QJsonArray>

UserMgr::~UserMgr()
{

}

void UserMgr::setToken(QString token)
{
    _token = token;
}

void UserMgr::setUserInfo(std::shared_ptr<UserInfo> userInfo)
{
    _userInfo = userInfo;
}

int UserMgr::getUid()
{
    return _userInfo->_uid;
}

QString UserMgr::getNick()
{
    return _userInfo->_nick;
}

QString UserMgr::getDesc()
{
    return _userInfo->_desc;
}

QString UserMgr::getIcon()
{
    return _userInfo->_icon;
}

std::shared_ptr<UserInfo> UserMgr::getUserInfo()
{
    return _userInfo;
}

void UserMgr::appendFriendList(QJsonArray array)
{
    // 遍历 QJsonArray 并输出每个元素
    for (const QJsonValue& value : array) {
        auto uid = value["uid"].toInt();
        auto email = value["email"].toString();
        auto nick = value["nick"].toString();
        auto desc = value["desc"].toString();
        auto sex = value["sex"].toInt();
        auto icon = value["icon"].toString();

        auto info = std::make_shared<UserInfo>(uid, email, nick, desc, sex, icon, "");
        _friendList.push_back(info);
        _friendMap.insert(uid, info);
    }
}

std::vector<std::shared_ptr<UserInfo>> UserMgr::getFriendList()
{
    return _friendList;
}

std::shared_ptr<UserInfo> UserMgr::GetFriendById(int uid)
{
    auto find_it = _friendMap.find(uid);
    if(find_it == _friendMap.end()){
        return nullptr;
    }

    return *find_it;
}

int UserMgr::GetLastChatThreadId()
{
    return _last_chat_thread_id;
}

void UserMgr::SetLastChatThreadId(int id)
{
    _last_chat_thread_id = id;
}

void UserMgr::AddChatThreadData(std::shared_ptr<ChatThreadData> chat_thread_data, int other_uid)
{
    //建立会话id到数据的映射关系
    _chat_map[chat_thread_data->GetThreadId()] = chat_thread_data;
    //存储会话列表
    _chat_thread_ids.push_back(chat_thread_data->GetThreadId());
    if (other_uid) {
        //将对方uid和会话id关联
        _uid_to_thread_id[other_uid] = chat_thread_data->GetThreadId();
    }
}

int UserMgr::GetThreadIdByUid(int uid)
{
    auto iter = _uid_to_thread_id.find(uid);
    if (iter == _uid_to_thread_id.end()){
        return -1;
    }

    return iter.value();
}

std::shared_ptr<ChatThreadData> UserMgr::GetChatThreadByThreadId(int thread_id)
{
    auto find_iter = _chat_map.find(thread_id);
    if (find_iter != _chat_map.end()) {
        return find_iter.value();
    }
    return nullptr;
}

std::shared_ptr<ChatThreadData> UserMgr::GetChatThreadByUid(int uid)
{
    auto iter = _uid_to_thread_id.find(uid);
    if (iter == _uid_to_thread_id.end()) {
        return nullptr;
    }

    auto chat_iter = _chat_map.find(iter.value());
    if(chat_iter == _chat_map.end()) {
        return nullptr;
    }

    return chat_iter.value();
}

UserMgr::UserMgr()
{

}
