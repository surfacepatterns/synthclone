/*
 * synthclone - Synthesizer-cloning software
 * Copyright (C) 2011-2012 Devin Anderson
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

#ifndef __MENUMANAGER_H__
#define __MENUMANAGER_H__

#include <QtCore/QStringList>

#include <synthclone/menuaction.h>
#include <synthclone/menuseparator.h>

#include "mainview.h"
#include "session.h"

class MenuManager: public QObject {

    Q_OBJECT

public:

    MenuManager(MainView &mainView, Session &session, QObject *parent=0);

    ~MenuManager();

public slots:

    const synthclone::Registration &
    addMenuAction(synthclone::MenuAction *action, synthclone::Menu menu,
                  const QStringList &subMenus=QStringList());

    const synthclone::Registration &
    addMenuAction(synthclone::MenuAction *action,
                  const synthclone::Effect *effect,
                  const QStringList &subMenus=QStringList());

    const synthclone::Registration &
    addMenuAction(synthclone::MenuAction *action,
                  const synthclone::Sampler *sampler,
                  const QStringList &subMenus=QStringList());

    const synthclone::Registration &
    addMenuAction(synthclone::MenuAction *action,
                  const synthclone::Target *target,
                  const QStringList &subMenus=QStringList());

    const synthclone::Registration &
    addMenuSeparator(synthclone::MenuSeparator *separator,
                     synthclone::Menu menu,
                     const QStringList &subMenus=QStringList());

    const synthclone::Registration &
    addMenuSeparator(synthclone::MenuSeparator *separator,
                     const synthclone::Effect *effect,
                     const QStringList &subMenus=QStringList());

    const synthclone::Registration &
    addMenuSeparator(synthclone::MenuSeparator *separator,
                     const synthclone::Sampler *sampler,
                     const QStringList &subMenus=QStringList());

    const synthclone::Registration &
    addMenuSeparator(synthclone::MenuSeparator *separator,
                     const synthclone::Target *target,
                     const QStringList &subMenus=QStringList());

    void
    removeMenuAction(const synthclone::MenuAction *action);

    void
    removeMenuSeparator(const synthclone::MenuSeparator *separator);

signals:

    void
    addingMenuAction(const synthclone::MenuAction *action,
                     synthclone::Menu menu, const QStringList &subMenus);

    void
    addingMenuAction(const synthclone::MenuAction *action,
                     const synthclone::Effect *effect,
                     const QStringList &subMenus);

    void
    addingMenuAction(const synthclone::MenuAction *action,
                     const synthclone::Sampler *sampler,
                     const QStringList &subMenus);

    void
    addingMenuAction(const synthclone::MenuAction *action,
                     const synthclone::Target *target,
                     const QStringList &subMenus);

    void
    addingMenuSeparator(const synthclone::MenuSeparator *separator,
                        synthclone::Menu menu, const QStringList &subMenus);

    void
    addingMenuSeparator(const synthclone::MenuSeparator *separator,
                        const synthclone::Effect *effect,
                        const QStringList &subMenus);

    void
    addingMenuSeparator(const synthclone::MenuSeparator *separator,
                        const synthclone::Sampler *sampler,
                        const QStringList &subMenus);

    void
    addingMenuSeparator(const synthclone::MenuSeparator *separator,
                        const synthclone::Target *target,
                        const QStringList &subMenus);

    void
    menuActionAdded(const synthclone::MenuAction *action, synthclone::Menu menu,
                    const QStringList &subMenus);

    void
    menuActionAdded(const synthclone::MenuAction *action,
                    const synthclone::Effect *effect,
                    const QStringList &subMenus);

    void
    menuActionAdded(const synthclone::MenuAction *action,
                    const synthclone::Sampler *sampler,
                    const QStringList &subMenus);

    void
    menuActionAdded(const synthclone::MenuAction *action,
                    const synthclone::Target *target,
                    const QStringList &subMenus);

    void
    menuActionRemoved(const synthclone::MenuAction *action,
                      synthclone::Menu menu, const QStringList &subMenus);

    void
    menuActionRemoved(const synthclone::MenuAction *action,
                      const synthclone::Effect *effect,
                      const QStringList &subMenus);

    void
    menuActionRemoved(const synthclone::MenuAction *action,
                      const synthclone::Sampler *sampler,
                      const QStringList &subMenus);

    void
    menuActionRemoved(const synthclone::MenuAction *action,
                      const synthclone::Target *target,
                      const QStringList &subMenus);

    void
    menuSeparatorAdded(const synthclone::MenuSeparator *separator,
                       synthclone::Menu menu, const QStringList &subMenus);

    void
    menuSeparatorAdded(const synthclone::MenuSeparator *separator,
                       const synthclone::Effect *effect,
                       const QStringList &subMenus);

    void
    menuSeparatorAdded(const synthclone::MenuSeparator *separator,
                       const synthclone::Sampler *sampler,
                       const QStringList &subMenus);

    void
    menuSeparatorAdded(const synthclone::MenuSeparator *separator,
                       const synthclone::Target *target,
                       const QStringList &subMenus);

    void
    menuSeparatorRemoved(const synthclone::MenuSeparator *separator,
                         synthclone::Menu menu, const QStringList &subMenus);

    void
    menuSeparatorRemoved(const synthclone::MenuSeparator *separator,
                         const synthclone::Effect *effect,
                         const QStringList &subMenus);

    void
    menuSeparatorRemoved(const synthclone::MenuSeparator *separator,
                         const synthclone::Sampler *sampler,
                         const QStringList &subMenus);

    void
    menuSeparatorRemoved(const synthclone::MenuSeparator *separator,
                         const synthclone::Target *target,
                         const QStringList &subMenus);

    void
    removingMenuAction(const synthclone::MenuAction *action,
                       synthclone::Menu menu, const QStringList &subMenus);

    void
    removingMenuAction(const synthclone::MenuAction *action,
                       const synthclone::Effect *effect,
                       const QStringList &subMenus);

    void
    removingMenuAction(const synthclone::MenuAction *action,
                       const synthclone::Sampler *sampler,
                       const QStringList &subMenus);

    void
    removingMenuAction(const synthclone::MenuAction *action,
                       const synthclone::Target *target,
                       const QStringList &subMenus);

    void
    removingMenuSeparator(const synthclone::MenuSeparator *separator,
                          synthclone::Menu menu, const QStringList &subMenus);

    void
    removingMenuSeparator(const synthclone::MenuSeparator *separator,
                          const synthclone::Effect *effect,
                          const QStringList &subMenus);

    void
    removingMenuSeparator(const synthclone::MenuSeparator *separator,
                          const synthclone::Sampler *sampler,
                          const QStringList &subMenus);

    void
    removingMenuSeparator(const synthclone::MenuSeparator *separator,
                          const synthclone::Target *target,
                          const QStringList &subMenus);

private:

    enum MenuBaseType {
        MENUBASETYPE_EFFECT,
        MENUBASETYPE_MENU,
        MENUBASETYPE_SAMPLER,
        MENUBASETYPE_TARGET
    };

    typedef QList<const synthclone::MenuAction *> MenuActionList;
    typedef QList<const synthclone::MenuSeparator *> MenuSeparatorList;

    struct ComponentData {
        MenuActionList menuActions;
        MenuSeparatorList menuSeparators;
    };

    struct MenuItemData {
        union {
            synthclone::Menu menu;
            const synthclone::Effect *effect;
            const synthclone::Sampler *sampler;
            const synthclone::Target *target;
        } menuBase;
        MenuBaseType menuBaseType;
        Registration *registration;
        QStringList subMenus;
        MenuLeafViewlet *viewlet;
    };

    typedef QMap<const synthclone::Effect *, ComponentData *> EffectDataMap;
    typedef QMap<const synthclone::MenuItem *, MenuItemData *> MenuItemDataMap;
    typedef QMap<const synthclone::Target *, ComponentData *> TargetDataMap;

    MenuItemData *
    addMenuAction(synthclone::MenuAction *action, const QStringList &subMenus,
                  MenuViewlet *topMenu);

    MenuItemData *
    addMenuItem(synthclone::MenuItem *item, const QStringList &subMenus,
                MenuViewlet *menu, MenuLeafViewlet *viewlet);

    MenuItemData *
    addMenuSeparator(synthclone::MenuSeparator *separator,
                     const QStringList &subMenus, MenuViewlet *topMenu);

    MenuViewlet *
    addSubMenus(MenuViewlet *topMenu, const QStringList &subMenus);

    void
    cleanupMenus(MenuViewlet *menu, const MenuViewlet *topMenu);

    void
    destroyMenuItems(MenuViewlet *menu);

    MenuViewlet *
    getMenuViewlet(synthclone::Menu menu);

    void
    removeComponentMenuItems(ComponentData *data);

    void
    removeMenuAction(const synthclone::MenuAction *action, MenuItemData *data,
                     MenuViewlet *topMenu);

    void
    removeMenuItem(const synthclone::MenuItem *item, MenuItemData *data,
                   MenuViewlet *topMenu);

    void
    removeMenuSeparator(const synthclone::MenuSeparator *separator,
                        MenuItemData *data, MenuViewlet *topMenu);

    EffectDataMap effectDataMap;
    MainView &mainView;
    MenuItemDataMap menuItemDataMap;
    ComponentData samplerData;
    Session &session;
    TargetDataMap targetDataMap;

private slots:

    void
    handleEffectAddition(const synthclone::Effect *effect, int index);

    void
    handleEffectRemoval(const synthclone::Effect *effect, int index);

    void
    handleSamplerRemoval(const synthclone::Sampler *sampler);

    void
    handleTargetAddition(const synthclone::Target *target, int index);

    void
    handleTargetRemoval(const synthclone::Target *target, int index);

};

#endif
