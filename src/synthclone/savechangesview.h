#ifndef __SAVECHANGESVIEW_H__
#define __SAVECHANGESVIEW_H__

#include <QtGui/QPushButton>

#include "dialogview.h"

class SaveChangesView: public DialogView {

    Q_OBJECT

public:

    explicit
    SaveChangesView(QObject *parent=0);

    ~SaveChangesView();

signals:

    void
    discardRequest();

    void
    saveRequest();

private:

    QPushButton *cancelButton;
    QPushButton *discardButton;
    QPushButton *saveButton;

};

#endif
