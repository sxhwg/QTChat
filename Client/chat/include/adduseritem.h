#ifndef ADDUSERITEM_H
#define ADDUSERITEM_H

#include "listitembase.h"

namespace Ui {
class AddUserItem;
}

class AddUserItem : public ListItemBase
{
    Q_OBJECT

public:
    explicit AddUserItem(QWidget *parent = nullptr);
    ~AddUserItem();

    QSize sizeHint() const override;

private:
    Ui::AddUserItem *ui;
};

#endif // ADDUSERITEM_H
