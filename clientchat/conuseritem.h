#ifndef CONUSERITEM_H
#define CONUSERITEM_H

#include <QWidget>
#include "listitembase.h"
#include "userdata.h"

namespace Ui {
class ConUserItem;
}

class ConUserItem : public ListItemBase
{
    Q_OBJECT

public:
    explicit ConUserItem(QWidget *parent = nullptr);
    ~ConUserItem();

    void SetInfo(std::shared_ptr<UserInfo> friendInfo);

    std::shared_ptr<UserInfo> GetUserInfo();

private:
    Ui::ConUserItem *ui;

    std::shared_ptr<UserInfo> _friendInfo;
};

#endif // CONUSERITEM_H
