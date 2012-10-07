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

#include <cassert>

#include <QtCore/QDebug>
#include <QtCore/QScopedPointer>

#include <synthclone/util.h>

#include "menumanager.h"

MenuManager::MenuManager(MainView &mainView, Session &session, QObject *parent):
    QObject(parent),
    mainView(mainView),
    session(session)
{
    connect(&session, SIGNAL(effectAdded(const synthclone::Effect *, int)),
            SLOT(handleEffectAddition(const synthclone::Effect *, int)));
    connect(&session, SIGNAL(removingEffect(const synthclone::Effect *, int)),
            SLOT(handleEffectRemoval(const synthclone::Effect *, int)));
    connect(&session, SIGNAL(removingSampler(const synthclone::Sampler *)),
            SLOT(handleSamplerRemoval(const synthclone::Sampler *)));
    connect(&session, SIGNAL(removingTarget(const synthclone::Target *, int)),
            SLOT(handleTargetRemoval(const synthclone::Target *, int)));
    connect(&session, SIGNAL(targetAdded(const synthclone::Target *, int)),
            SLOT(handleTargetAddition(const synthclone::Target *, int)));
}

MenuManager::~MenuManager()
{
    // Empty
}

MenuManager::MenuItemData *
MenuManager::addMenuAction(synthclone::MenuAction *action,
                           const QStringList &subMenus, MenuViewlet *topMenu)
{
    assert(action);
    assert(topMenu);
    MenuViewlet *menu = addSubMenus(topMenu, subMenus);
    MenuActionViewlet *viewlet = mainView.createMenuActionViewlet(menu);
    viewlet->setDescription(action->getDescription());
    viewlet->setEnabled(action->isEnabled());
    viewlet->setText(action->getText());

    connect(action, SIGNAL(descriptionChanged(const QString)),
            viewlet, SLOT(setDescription(const QString)));
    connect(action, SIGNAL(enabledChanged(bool)),
            viewlet, SLOT(setEnabled(bool)));
    connect(action, SIGNAL(textChanged(const QString)),
            viewlet, SLOT(setText(const QString)));

    connect(viewlet, SIGNAL(triggered()), action, SIGNAL(triggered()));

    return addMenuItem(action, subMenus, menu, viewlet);
}

const synthclone::Registration &
MenuManager::addMenuAction(synthclone::MenuAction *action,
                           synthclone::Menu menu, const QStringList &subMenus)
{
    CONFIRM(action, tr("action is set to NULL"));
    CONFIRM(! menuItemDataMap.contains(action),
            tr("menu action is already registered with menu manager"));

    emit addingMenuAction(action, menu, subMenus);
    MenuItemData *data = addMenuAction(action, subMenus, getMenuViewlet(menu));
    data->menuBase.menu = menu;
    data->menuBaseType = MENUBASETYPE_MENU;
    emit menuActionAdded(action, menu, subMenus);
    return *(data->registration);
}

const synthclone::Registration &
MenuManager::addMenuAction(synthclone::MenuAction *action,
                           const synthclone::Effect *effect,
                           const QStringList &subMenus)
{
    CONFIRM(action, tr("action is set to NULL"));
    CONFIRM(! menuItemDataMap.contains(action),
            tr("menu action is already registered with menu manager"));
    CONFIRM(effect, tr("effect is set to NULL"));

    ComponentData *effectData = effectDataMap.value(effect, 0);

    CONFIRM(effectData, tr("effect is not registered with session"));

    emit addingMenuAction(action, effect, subMenus);
    int index = session.getEffectIndex(effect);
    MenuViewlet *viewlet = qobject_cast<MenuViewlet *>
        (mainView.getComponentViewlet()->getEffectMenuViewlet(index));
    MenuItemData *data = addMenuAction(action, subMenus, viewlet);
    data->menuBase.effect = effect;
    data->menuBaseType = MENUBASETYPE_EFFECT;
    effectData->menuActions.append(action);
    emit menuActionAdded(action, effect, subMenus);
    return *(data->registration);
}

const synthclone::Registration &
MenuManager::addMenuAction(synthclone::MenuAction *action,
                           const synthclone::Sampler *sampler,
                           const QStringList &subMenus)
{
    CONFIRM(action, tr("action is set to NULL"));
    CONFIRM(! menuItemDataMap.contains(action),
            tr("menu action is already registered with menu manager"));
    CONFIRM(sampler, tr("sampler is set to NULL"));
    CONFIRM(sampler == session.getSampler(),
            tr("sampler is not registered with session"));

    emit addingMenuAction(action, sampler, subMenus);
    MenuViewlet *viewlet = qobject_cast<MenuViewlet *>
        (mainView.getComponentViewlet()->getSamplerMenuViewlet());
    MenuItemData *data = addMenuAction(action, subMenus, viewlet);
    data->menuBase.sampler = sampler;
    data->menuBaseType = MENUBASETYPE_SAMPLER;
    samplerData.menuActions.append(action);
    emit menuActionAdded(action, sampler, subMenus);
    return *(data->registration);
}

const synthclone::Registration &
MenuManager::addMenuAction(synthclone::MenuAction *action,
                           const synthclone::Target *target,
                           const QStringList &subMenus)
{
    CONFIRM(action, tr("action is set to NULL"));
    CONFIRM(! menuItemDataMap.contains(action),
            tr("menu action is already registered with menu manager"));
    CONFIRM(target, tr("target is set to NULL"));

    ComponentData *targetData = targetDataMap.value(target, 0);

    CONFIRM(targetData, tr("target is not registered with session"));

    emit addingMenuAction(action, target, subMenus);
    int index = session.getTargetIndex(target);
    MenuViewlet *viewlet = qobject_cast<MenuViewlet *>
        (mainView.getComponentViewlet()->getTargetMenuViewlet(index));
    MenuItemData *data = addMenuAction(action, subMenus, viewlet);
    data->menuBase.target = target;
    data->menuBaseType = MENUBASETYPE_TARGET;
    targetData->menuActions.append(action);
    emit menuActionAdded(action, target, subMenus);
    return *(data->registration);
}

MenuManager::MenuItemData *
MenuManager::addMenuItem(synthclone::MenuItem *item,
                         const QStringList &subMenus, MenuViewlet *menu,
                         MenuLeafViewlet *viewlet)
{
    assert(item);
    assert(menu);
    assert(viewlet);
    menu->add(viewlet);
    viewlet->setVisible(item->isVisible());
    connect(item, SIGNAL(visibilityChanged(bool)),
            viewlet, SLOT(setVisible(bool)));

    MenuItemData *data = new MenuItemData();
    data->registration = new Registration(item, this);
    data->subMenus = subMenus;
    data->viewlet = viewlet;
    menuItemDataMap.insert(item, data);
    return data;
}

MenuManager::MenuItemData *
MenuManager::addMenuSeparator(synthclone::MenuSeparator *separator,
                              const QStringList &subMenus, MenuViewlet *topMenu)
{
    assert(separator);
    assert(topMenu);
    MenuViewlet *menu = addSubMenus(topMenu, subMenus);
    return addMenuItem(separator, subMenus, menu,
                       mainView.createMenuSeparatorViewlet(menu));
}

const synthclone::Registration &
MenuManager::addMenuSeparator(synthclone::MenuSeparator *separator,
                              synthclone::Menu menu,
                              const QStringList &subMenus)
{
    CONFIRM(separator, tr("separator is set to NULL"));
    CONFIRM(! menuItemDataMap.contains(separator),
            tr("menu separator is already registered with menu manager"));

    emit addingMenuSeparator(separator, menu, subMenus);
    MenuItemData *data = addMenuSeparator(separator, subMenus,
                                          getMenuViewlet(menu));
    data->menuBase.menu = menu;
    data->menuBaseType = MENUBASETYPE_MENU;
    emit menuSeparatorAdded(separator, menu, subMenus);
    return *(data->registration);
}

const synthclone::Registration &
MenuManager::addMenuSeparator(synthclone::MenuSeparator *separator,
                              const synthclone::Effect *effect,
                              const QStringList &subMenus)
{
    CONFIRM(separator, tr("separator is set to NULL"));
    CONFIRM(! menuItemDataMap.contains(separator),
            tr("menu separator is already registered with menu manager"));
    CONFIRM(effect, tr("effect is set to NULL"));

    ComponentData *effectData = effectDataMap.value(effect, 0);

    CONFIRM(effectData, tr("effect is not registered with session"));

    emit addingMenuSeparator(separator, effect, subMenus);
    int index = session.getEffectIndex(effect);
    MenuViewlet *viewlet = qobject_cast<MenuViewlet *>
        (mainView.getComponentViewlet()->getEffectMenuViewlet(index));
    MenuItemData *data = addMenuSeparator(separator, subMenus, viewlet);
    data->menuBase.effect = effect;
    data->menuBaseType = MENUBASETYPE_EFFECT;
    effectData->menuSeparators.append(separator);
    emit menuSeparatorAdded(separator, effect, subMenus);
    return *(data->registration);
}

const synthclone::Registration &
MenuManager::addMenuSeparator(synthclone::MenuSeparator *separator,
                              const synthclone::Sampler *sampler,
                              const QStringList &subMenus)
{
    CONFIRM(separator, tr("separator is set to NULL"));
    CONFIRM(! menuItemDataMap.contains(separator),
            tr("menu separator is already registered with menu manager"));
    CONFIRM(sampler, tr("sampler is set to NULL"));
    CONFIRM(sampler == session.getSampler(),
            tr("sampler is not registered with session"));

    emit addingMenuSeparator(separator, sampler, subMenus);
    MenuViewlet *viewlet = qobject_cast<MenuViewlet *>
        (mainView.getComponentViewlet()->getSamplerMenuViewlet());
    MenuItemData *data = addMenuSeparator(separator, subMenus, viewlet);
    data->menuBase.sampler = sampler;
    data->menuBaseType = MENUBASETYPE_SAMPLER;
    samplerData.menuSeparators.append(separator);
    emit menuSeparatorAdded(separator, sampler, subMenus);
    return *(data->registration);
}

const synthclone::Registration &
MenuManager::addMenuSeparator(synthclone::MenuSeparator *separator,
                              const synthclone::Target *target,
                              const QStringList &subMenus)
{
    CONFIRM(separator, tr("separator is set to NULL"));
    CONFIRM(! menuItemDataMap.contains(separator),
            tr("menu separator is already registered with menu manager"));
    CONFIRM(target, tr("target is set to NULL"));

    ComponentData *targetData = targetDataMap.value(target, 0);

    CONFIRM(targetData, tr("target is not registered with session"));

    emit addingMenuSeparator(separator, target, subMenus);
    int index = session.getTargetIndex(target);
    MenuViewlet *viewlet = qobject_cast<MenuViewlet *>
        (mainView.getComponentViewlet()->getTargetMenuViewlet(index));
    MenuItemData *data = addMenuSeparator(separator, subMenus, viewlet);
    data->menuBase.target = target;
    data->menuBaseType = MENUBASETYPE_TARGET;
    targetData->menuSeparators.append(separator);
    emit menuSeparatorAdded(separator, target, subMenus);
    return *(data->registration);
}

MenuViewlet *
MenuManager::addSubMenus(MenuViewlet *topMenu, const QStringList &subMenus)
{
    assert(topMenu);
    if (! topMenu->getChildCount()) {
        topMenu->setVisible(true);
    }
    MenuViewlet *menu = topMenu;
    int subMenuCount = subMenus.count();
    for (int i = 0; i < subMenuCount; i++) {
        QString text = subMenus[i];
        MenuViewlet *subMenu = menu->getSubMenuViewlet(text);
        if (! subMenu) {
            subMenu = mainView.createMenuViewlet(text, menu);
            menu->add(subMenu);
        }
        menu = subMenu;
    }
    return menu;
}

void
MenuManager::cleanupMenus(MenuViewlet *menu, const MenuViewlet *topMenu)
{
    while ((menu != topMenu) && (! menu->getChildCount())) {
        MenuViewlet *parentMenu = qobject_cast<MenuViewlet *>(menu->parent());
        parentMenu->remove(menu);
        mainView.destroyMenuViewlet(menu);
        menu = parentMenu;
    }
}

void
MenuManager::destroyMenuItems(MenuViewlet *menu)
{
    for (int i = menu->getChildCount() - 1; i >= 0; i--) {
        MenuItemViewlet *item = menu->getChild(i);
        MenuViewlet *menuViewlet = qobject_cast<MenuViewlet *>(item);
        if (menuViewlet) {
            destroyMenuItems(menuViewlet);
            mainView.destroyMenuViewlet(menuViewlet);
            continue;
        }
        MenuLeafViewlet *leaf = qobject_cast<MenuLeafViewlet *>(item);
        assert(leaf);
        menu->remove(leaf);
        MenuActionViewlet *action = qobject_cast<MenuActionViewlet *>(item);
        if (action) {
            mainView.destroyMenuActionViewlet(action);
            continue;
        }
        MenuSeparatorViewlet *separator =
            qobject_cast<MenuSeparatorViewlet *>(item);
        assert(separator);
        mainView.destroyMenuSeparatorViewlet(separator);
    }
}

MenuViewlet *
MenuManager::getMenuViewlet(synthclone::Menu menu)
{
    MenuViewlet *viewlet;
    switch (menu) {
    case synthclone::MENU_ADD_EFFECT:
        viewlet = mainView.getComponentViewlet()->getEffectAddMenuViewlet();
        break;
    case synthclone::MENU_ADD_SAMPLER:
        viewlet = mainView.getComponentViewlet()->getSamplerAddMenuViewlet();
        break;
    case synthclone::MENU_ADD_TARGET:
        viewlet = mainView.getComponentViewlet()->getTargetAddMenuViewlet();
        break;
    case synthclone::MENU_HELP:
        viewlet = mainView.getHelpViewlet()->getMenuViewlet();
        break;
    case synthclone::MENU_SESSION:
        viewlet = mainView.getSessionViewlet()->getMenuViewlet();
        break;
    case synthclone::MENU_TOOLS:
        viewlet = mainView.getToolViewlet()->getMenuViewlet();
        break;
    case synthclone::MENU_VIEW:
        viewlet = mainView.getViewViewlet()->getMenuViewlet();
        break;
    case synthclone::MENU_ZONES:
        viewlet = mainView.getZoneViewlet()->getMenuViewlet();
        break;
    default:
        assert(false);
    }
    return viewlet;
}

void
MenuManager::handleEffectAddition(const synthclone::Effect *effect,
                                  int /*index*/)
{
    effectDataMap.insert(effect, new ComponentData());
}

void
MenuManager::handleEffectRemoval(const synthclone::Effect *effect,
                                 int /*index*/)
{
    ComponentData *data = effectDataMap.value(effect, 0);
    removeComponentMenuItems(data);
    delete effectDataMap.take(effect);
}

void
MenuManager::handleSamplerRemoval(const synthclone::Sampler */*sampler*/)
{
    removeComponentMenuItems(&samplerData);
}

void
MenuManager::handleTargetAddition(const synthclone::Target *target,
                                  int /*index*/)
{
    targetDataMap.insert(target, new ComponentData());
}

void
MenuManager::handleTargetRemoval(const synthclone::Target *target,
                                 int /*index*/)
{
    ComponentData *data = targetDataMap.value(target, 0);
    removeComponentMenuItems(data);
    delete targetDataMap.take(target);
}

void
MenuManager::removeComponentMenuItems(ComponentData *data)
{
    assert(data);
    int i;
    MenuActionList &actions = data->menuActions;
    for (i = actions.count() - 1; i >= 0; i--) {
        removeMenuAction(actions[i]);
    }
    MenuSeparatorList &separators = data->menuSeparators;
    for (i = separators.count() - 1; i >= 0; i--) {
        removeMenuSeparator(separators[i]);
    }
}

void
MenuManager::removeMenuAction(const synthclone::MenuAction *action,
                              MenuItemData *data, MenuViewlet *topMenu)
{
    assert(action);
    assert(topMenu);
    removeMenuItem(action, data, topMenu);
    MenuActionViewlet *viewlet =
        qobject_cast<MenuActionViewlet *>(data->viewlet);
    disconnect(action, SIGNAL(descriptionChanged(const QString)),
               viewlet, SLOT(setDescription(const QString)));
    disconnect(action, SIGNAL(enabledChanged(bool)),
               viewlet, SLOT(setEnabled(bool)));
    disconnect(action, SIGNAL(textChanged(const QString)),
               viewlet, SLOT(setText(const QString)));
    disconnect(viewlet, SIGNAL(triggered()), action, SIGNAL(triggered()));
    mainView.destroyMenuActionViewlet(viewlet);
}

void
MenuManager::removeMenuAction(const synthclone::MenuAction *action)
{
    CONFIRM(action, tr("action is set to NULL"));

    MenuItemData *data = menuItemDataMap.value(action, 0);

    CONFIRM(data, tr("menu action is not registered with menu manager"));

    QScopedPointer<Registration> registrationPtr(data->registration);
    const synthclone::Effect *effect;
    int index;
    synthclone::Menu menu;
    bool removed;
    const synthclone::Sampler *sampler;
    const synthclone::Target *target;
    QStringList &subMenus = data->subMenus;
    MenuViewlet *viewlet;
    switch (data->menuBaseType) {
    case MENUBASETYPE_EFFECT:
        effect = data->menuBase.effect;
        emit removingMenuAction(action, effect, subMenus);
        removed = effectDataMap.value(effect)->menuActions.removeOne(action);
        assert(removed);
        index = session.getEffectIndex(effect);
        viewlet = qobject_cast<MenuViewlet *>
            (mainView.getComponentViewlet()->getEffectMenuViewlet(index));
        removeMenuAction(action, data, viewlet);
        emit menuActionRemoved(action, effect, subMenus);
        break;
    case MENUBASETYPE_MENU:
        menu = data->menuBase.menu;
        emit removingMenuAction(action, menu, subMenus);
        removeMenuAction(action, data, getMenuViewlet(menu));
        emit menuActionRemoved(action, menu, subMenus);
        break;
    case MENUBASETYPE_SAMPLER:
        sampler = data->menuBase.sampler;
        emit removingMenuAction(action, sampler, subMenus);
        removed = samplerData.menuActions.removeOne(action);
        assert(removed);
        viewlet = mainView.getComponentViewlet()->getSamplerMenuViewlet();
        removeMenuAction(action, data, viewlet);
        emit menuActionRemoved(action, sampler, subMenus);
        break;
    case MENUBASETYPE_TARGET:
        target = data->menuBase.target;
        emit removingMenuAction(action, target, subMenus);
        removed = targetDataMap.value(target)->menuActions.removeOne(action);
        assert(removed);
        index = session.getTargetIndex(target);
        viewlet = qobject_cast<MenuViewlet *>
            (mainView.getComponentViewlet()->getTargetMenuViewlet(index));
        removeMenuAction(action, data, viewlet);
        emit menuActionRemoved(action, target, subMenus);
    }
    delete data;
}

void
MenuManager::removeMenuItem(const synthclone::MenuItem *item,
                            MenuItemData *data, MenuViewlet *topMenu)
{
    assert(item);
    assert(topMenu);
    MenuViewlet *menu = topMenu;
    const QStringList &subMenus = data->subMenus;
    int subMenuCount = subMenus.count();
    for (int i = 0; i < subMenuCount; i++) {
        const QString &subMenuText = subMenus[i];
        MenuViewlet *subMenu = menu->getSubMenuViewlet(subMenuText);
        assert(subMenu);
        menu = subMenu;
    }
    MenuLeafViewlet *viewlet = data->viewlet;
    menu->remove(viewlet);
    cleanupMenus(menu, topMenu);
    if (! topMenu->getChildCount()) {
        topMenu->setVisible(false);
    }

    qDebug() << "removeMenuItem:";
    qDebug() << "\titem: " << item;
    qDebug() << "\tviewlet: " << viewlet;

    disconnect(item, SIGNAL(visibilityChanged(bool)),
               viewlet, SLOT(setVisible(bool)));
    menuItemDataMap.remove(item);
}

void
MenuManager::removeMenuSeparator(const synthclone::MenuSeparator *separator,
                                 MenuItemData *data, MenuViewlet *topMenu)
{
    assert(separator);
    assert(topMenu);
    removeMenuItem(separator, data, topMenu);
    mainView.destroyMenuSeparatorViewlet
        (qobject_cast<MenuSeparatorViewlet *>(data->viewlet));
}

void
MenuManager::removeMenuSeparator(const synthclone::MenuSeparator *separator)
{
    CONFIRM(separator, tr("separator is set to NULL"));

    MenuItemData *data = menuItemDataMap.value(separator, 0);

    CONFIRM(data, tr("menu separator is not registered with menu manager"));

    QScopedPointer<Registration> registrationPtr(data->registration);
    const synthclone::Effect *effect;
    int index;
    synthclone::Menu menu;
    bool removed;
    const synthclone::Sampler *sampler;
    const synthclone::Target *target;
    QStringList &subMenus = data->subMenus;
    MenuViewlet *viewlet;
    switch (data->menuBaseType) {
    case MENUBASETYPE_EFFECT:
        effect = data->menuBase.effect;
        emit removingMenuSeparator(separator, effect, subMenus);
        removed = effectDataMap.value(effect)->
            menuSeparators.removeOne(separator);
        assert(removed);
        index = session.getEffectIndex(effect);
        viewlet = qobject_cast<MenuViewlet *>
            (mainView.getComponentViewlet()->getEffectMenuViewlet(index));
        removeMenuSeparator(separator, data, viewlet);
        emit menuSeparatorRemoved(separator, effect, subMenus);
        break;
    case MENUBASETYPE_MENU:
        menu = data->menuBase.menu;
        emit removingMenuSeparator(separator, menu, subMenus);
        removeMenuSeparator(separator, data, getMenuViewlet(menu));
        emit menuSeparatorRemoved(separator, menu, subMenus);
        break;
    case MENUBASETYPE_SAMPLER:
        sampler = data->menuBase.sampler;
        emit removingMenuSeparator(separator, sampler, subMenus);
        removed = samplerData.menuSeparators.removeOne(separator);
        assert(removed);
        viewlet = mainView.getComponentViewlet()->getSamplerMenuViewlet();
        removeMenuSeparator(separator, data, viewlet);
        emit menuSeparatorRemoved(separator, sampler, subMenus);
        break;
    case MENUBASETYPE_TARGET:
        target = data->menuBase.target;
        emit removingMenuSeparator(separator, target, subMenus);
        removed = targetDataMap.value(target)->
            menuSeparators.removeOne(separator);
        assert(removed);
        index = session.getTargetIndex(target);
        viewlet = qobject_cast<MenuViewlet *>
            (mainView.getComponentViewlet()->getTargetMenuViewlet(index));
        removeMenuSeparator(separator, data, viewlet);
        emit menuSeparatorRemoved(separator, target, subMenus);
    }
    delete data;
}
