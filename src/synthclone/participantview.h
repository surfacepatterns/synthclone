#ifndef __PARTICIPANTVIEW_H__
#define __PARTICIPANTVIEW_H__

#include <QtGui/QDialog>
#include <QtGui/QPushButton>
#include <QtGui/QStandardItemModel>
#include <QtGui/QTreeView>

#include "dialogview.h"
#include "participantviewlet.h"

class ParticipantView: public DialogView {

    Q_OBJECT

public:

    explicit
    ParticipantView(QObject *parent=0);

    ~ParticipantView();

    void
    add(ParticipantViewlet *viewlet);

    ParticipantViewlet *
    createParticipantViewlet();

    void
    destroyParticipantViewlet(ParticipantViewlet *viewlet);

    ParticipantViewlet *
    getChild(int index);

    int
    getChildCount() const;

    void
    remove(ParticipantViewlet *viewlet);

public slots:

    void
    setVisible(bool visible);

private slots:

    void
    handleTreeViewActivation(const QModelIndex &index);

    void
    registerViewlet(ParticipantViewlet *viewlet);

    void
    unregisterViewlet(ParticipantViewlet *viewlet);

private:

    QPushButton *activateButton;
    QList<ParticipantViewlet *> children;
    QPushButton *closeButton;
    QMap<QStandardItem *, ParticipantViewlet *> itemViewletMap;
    QStandardItemModel treeModel;
    QTreeView *treeView;

};

#endif
