#include <synthclone/menuaction.h>

using synthclone::MenuAction;

MenuAction::MenuAction(QObject *parent):
    MenuItem(parent)
{
    initialize("", "");
}

MenuAction::MenuAction(const QString &text, QObject *parent):
    MenuItem(parent)
{
    initialize(text, "");
}

MenuAction::MenuAction(const QString &text, const QString &description,
                       QObject *parent):
    MenuItem(parent)
{
    initialize(text, description);
}

MenuAction::~MenuAction()
{
    // Empty
}

QString
MenuAction::getDescription() const
{
    return description;
}

QString
MenuAction::getText() const
{
    return text;
}

bool
MenuAction::isEnabled() const
{
    return enabled;
}

void
MenuAction::initialize(const QString &text, const QString &description)
{
    this->description = description;
    this->text = text;
    enabled = true;
}

void
MenuAction::setDescription(const QString &description)
{
    if (description != this->description) {
        this->description = description;
        emit descriptionChanged(description);
    }
}

void
MenuAction::setEnabled(bool enabled)
{
    if (enabled != this->enabled) {
        this->enabled = enabled;
        emit enabledChanged(enabled);
    }
}

void
MenuAction::setText(const QString &text)
{
    if (text != this->text) {
        this->text = text;
        emit textChanged(text);
    }
}

void
MenuAction::trigger()
{
    emit triggered();
}
