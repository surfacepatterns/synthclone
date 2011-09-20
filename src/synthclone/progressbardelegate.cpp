#include <cassert>

#include <QtCore/QLocale>
#include <QtGui/QApplication>
#include <QtGui/QStyleOptionProgressBar>

#include "progressbardelegate.h"

ProgressBarDelegate::ProgressBarDelegate(QObject *parent):
    QStyledItemDelegate(parent)
{
    // Empty
}

ProgressBarDelegate::~ProgressBarDelegate()
{
    // Empty
}

void
ProgressBarDelegate::paint(QPainter *painter,
                           const QStyleOptionViewItem &option,
                           const QModelIndex &index) const
{
    assert(index.isValid());
    QVariant variant = index.data();
    if (variant.isValid()) {
        float progress = variant.toFloat();
        assert((progress >= 0.0) && (progress <= 1.0));
        QLocale locale = QLocale::system();
        QStyleOptionProgressBar progressBarOption;
        progressBarOption.maximum = 10000;
        progressBarOption.minimum = 0;
        progressBarOption.progress = static_cast<int>(progress * 10000.0);
        progressBarOption.rect = option.rect;
        progressBarOption.text = tr("%1%2", "percentageFormat").
            arg(locale.toString(static_cast<int>(progress * 100.0))).
            arg(locale.percent());
        progressBarOption.textVisible = true;
        QApplication::style()->drawControl(QStyle::CE_ProgressBar,
                                           &progressBarOption, painter);
    } else {
        QStyledItemDelegate::paint(painter, option, index);
    }
}
