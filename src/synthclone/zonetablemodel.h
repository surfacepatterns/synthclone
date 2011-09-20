#ifndef __ZONETABLEMODEL_H__
#define __ZONETABLEMODEL_H__

#include <QtGui/QStandardItemModel>

class ZoneTableModel: public QStandardItemModel {

    Q_OBJECT

public:

    explicit
    ZoneTableModel(QObject *parent=0);

    ~ZoneTableModel();

    void
    sort(int column, Qt::SortOrder order=Qt::AscendingOrder);

signals:

    void
    sortRequest(int column, bool ascending);

};

#endif
