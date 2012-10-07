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

#ifndef __MENUACTIONDATACOMPARER_H__
#define __MENUACTIONDATACOMPARER_H__

#include "menuactiondata.h"

class MenuActionDataComparer: public QObject {

    Q_OBJECT

public:

    explicit
    MenuActionDataComparer(QObject *parent=0);

    MenuActionDataComparer(const MenuActionDataComparer &comparer,
                           QObject *parent=0);

    ~MenuActionDataComparer();

    bool
    operator()(const MenuActionData *data1, const MenuActionData *data2) const;

private:

    int
    compareSectionLists(const QStringList &sections1,
                        const QStringList &sections2, int length) const;

};

#endif
