/*
 * libsynthclone_lv2 - LV2 effect plugin for `synthclone`
 * Copyright (C) 2012 Devin Anderson
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

#include "menuactiondatacomparer.h"

MenuActionDataComparer::MenuActionDataComparer(QObject *parent):
    QObject(parent)
{
    // Empty
}

MenuActionDataComparer::
MenuActionDataComparer(const MenuActionDataComparer &/*comparer*/,
                       QObject *parent):
    QObject(parent)
{
    // Empty
}

MenuActionDataComparer::~MenuActionDataComparer()
{
    // Empty
}

bool
MenuActionDataComparer::operator()(const MenuActionData *data1,
                                   const MenuActionData *data2) const
{
    assert(data1);
    assert(data2);
    const QStringList &sections1 = data1->getSections();
    const QStringList &sections2 = data2->getSections();
    int length1 = sections1.count();
    int length2 = sections2.count();
    int result;
    if (length1 < length2) {
        result = compareSectionLists(sections1, sections2, length1);
    } else {
        result = compareSectionLists(sections1, sections2, length2);
        if (! result) {
            if (length1 != length2) {
                result = -1;
            } else {
                result = data1->getAction()->getText().
                    compare(data2->getAction()->getText());
            }
        }
    }
    return result < 0;
}

int
MenuActionDataComparer::compareSectionLists(const QStringList &sections1,
                                            const QStringList &sections2,
                                            int length) const
{
    int result = 0;
    for (int i = 0; i < length; i++) {
        result = sections1[i].compare(sections2[i]);
        if (result) {
            break;
        }
    }
    return result;
}
