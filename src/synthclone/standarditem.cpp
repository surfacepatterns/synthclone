#include "standarditem.h"

StandardItem::StandardItem():
    QStandardItem()
{
    // Empty
}

StandardItem::~StandardItem()
{
    // Empty
}

QStandardItem *
StandardItem::clone() const
{
    return new StandardItem();
}

QVariant
StandardItem::data(int role) const
{
    return role == Qt::EditRole ? editValue : QStandardItem::data(role);
}

void
StandardItem::setData(const QVariant &value, int role)
{
    if (role == Qt::EditRole) {
        if (editValue != value) {
            editValue = value;
            emitDataChanged();
        }
    } else {
        QStandardItem::setData(value, role);
    }
}
