#ifndef __STANDARDITEM_H__
#define __STANDARDITEM_H__

#include <QtGui/QStandardItem>

class StandardItem: public QStandardItem {

public:

    explicit
    StandardItem();

    ~StandardItem();

    QStandardItem *
    clone() const;

    QVariant
    data(int role=Qt::UserRole + 1) const;

    void
    setData(const QVariant &value, int role=Qt::UserRole + 1);

private:

    QVariant editValue;

};

#endif
