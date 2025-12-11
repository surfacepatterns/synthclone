module;

#include <Qt>
#include <QtCore/QAnyStringView>
#include <QtCore/QByteArray>
#include <QtCore/QByteArrayView>
#include <QtCore/QCoreApplication>
#include <QtCore/QDirIterator>
#include <QtCore/QEventLoop>
#include <QtCore/QFuture>
#include <QtCore/QFutureWatcher>
#include <QtCore/QList>
#include <QtCore/QMetaMethod>
#include <QtCore/QMetaObject>
#include <QtCore/QMetaProperty>
#include <QtCore/QObject>
#include <QtCore/QResource>
#include <QtCore/QSize>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QStringView>
#include <QtCore/QTemplatedMetaMethodReturnArgument>
#include <QtCore/QTextStream>
#include <QtCore/QThreadPool>
#include <QtCore/QVariant>

export module synthclone.external.qt.core;

export {

    using ::QAnyStringView;
    using ::QByteArray;
    using ::QByteArrayView;
    using ::QCoreApplication;
    using ::QDirIterator;
    using ::QEventLoop;
    using ::QFuture;
    using ::QFutureWatcher;
    using ::QList;
    using ::QMetaMethod;
    using ::QMetaObject;
    using ::QMetaProperty;
    using ::QObject;
    using ::QResource;
    using ::QSize;
    using ::QString;
    using ::QStringList;
    using ::QStringView;
    using ::QTemplatedMetaMethodReturnArgument;
    using ::QTextStream;
    using ::QThreadPool;
    using ::QVariant;

    using ::qobject_cast;
    using ::qReturnArg;

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
