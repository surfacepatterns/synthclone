/*
 * synthclone - Synthesizer-cloning software
 * Copyright (C) 2011 Devin Anderson
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 675 Mass
 * Ave, Cambridge, MA 02139, USA.
 */

#include <cassert>

#include <QtCore/QLocale>

#include <QtWidgets/QApplication>
#include <QtWidgets/QStyleOptionProgressBar>

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
