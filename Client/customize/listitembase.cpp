#include "listitembase.h"
#include <QStyleOption>
#include <QPainter>

ListItemBase::ListItemBase(QWidget *parent) : QWidget(parent)
{

}

void ListItemBase::SetItemType(ListItemType itemType)
{
    m_ItemType = itemType;
}

ListItemType ListItemBase::GetItemType()
{
    return m_ItemType;
}

void ListItemBase::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
