#ifndef __PARTICIPANTVIEWLET_H__
#define __PARTICIPANTVIEWLET_H__

#include <QtCore/QList>
#include <QtGui/QStandardItem>
#include <QtGui/QTreeView>

class ParticipantView;

class ParticipantViewlet: public QObject {

    Q_OBJECT

    friend class ParticipantView;

public:

    explicit
    ParticipantViewlet(QTreeView *treeView, QObject *parent=0);

    ~ParticipantViewlet();

    void
    add(ParticipantViewlet *viewlet);

    ParticipantViewlet *
    getChild(int index);

    int
    getChildCount() const;

    void
    remove(ParticipantViewlet *viewlet);

    void
    setActivated(bool activated);

    void
    setAuthor(const QString &author);

    void
    setMajorVersion(int majorVersion);

    void
    setMinorVersion(int minorVersion);

    void
    setName(const QString &name);

    void
    setRevision(int revision);

    void
    setSummary(const QString &summary);

signals:

    void
    activationChangeRequest(bool activation);

    void
    childAdded(ParticipantViewlet *viewlet);

    void
    childRemoved(ParticipantViewlet *viewlet);

private:

    void
    emitActivationChangeRequest(bool active);

    void
    updateVersion();

    QList<ParticipantViewlet *> children;
    QList<QStandardItem *> items;
    int majorVersion;
    int minorVersion;
    int revision;
    QTreeView *treeView;

};

#endif
