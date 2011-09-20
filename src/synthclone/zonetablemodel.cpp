#include "zonetablemodel.h"

ZoneTableModel::ZoneTableModel(QObject *parent):
    QStandardItemModel(parent)
{
    // Empty
}

ZoneTableModel::~ZoneTableModel()
{
    // Empty
}

void
ZoneTableModel::sort(int column, Qt::SortOrder order)
{
    emit sortRequest(column, order == Qt::AscendingOrder);
}
