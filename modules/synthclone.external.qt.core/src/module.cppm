module;

#include <Qt>
#include <QtCore/QAnyStringView>
#include <QtCore/QByteArray>
#include <QtCore/QEventLoop>
#include <QtCore/QMetaObject>
#include <QtCore/QObject>
#include <QtCore/QResource>
#include <QtCore/QString>
#include <QtCore/QStringView>
#include <QtCore/QTextStream>
#include <QtCore/QVariant>

export module synthclone.external.qt.core;

export {

    using ::QAnyStringView;
    using ::QByteArray;
    using ::QEventLoop;
    using ::QMetaObject;
    using ::QObject;
    using ::QResource;
    using ::QString;
    using ::QStringView;
    using ::QTextStream;
    using ::QVariant;

    namespace Qt {

        using Qt::ConnectionType;
        using Qt::FindChildOptions;

        using Qt::AutoConnection;
        using Qt::DirectConnection;
        using Qt::QueuedConnection;

        using Qt::FindChildrenRecursively;
        using Qt::FindDirectChildrenOnly;

    }

}
