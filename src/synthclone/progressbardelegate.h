#ifndef __PROGRESSBARDELEGATE_H__
#define __PROGRESSBARDELEGATE_H__

#include <QtGui/QStyledItemDelegate>

class ProgressBarDelegate: public QStyledItemDelegate {

    Q_OBJECT

public:

    explicit
    ProgressBarDelegate(QObject *parent=0);

    ~ProgressBarDelegate();

    void
    paint(QPainter *painter, const QStyleOptionViewItem &option,
          const QModelIndex &index) const;

};

#endif
