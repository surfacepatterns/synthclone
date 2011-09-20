#ifndef __DIALOGVIEW_H__
#define __DIALOGVIEW_H__

#include <QtGui/QDialog>

#include <synthclone/designerview.h>

class DialogView: public synthclone::DesignerView {

    Q_OBJECT

protected:

    explicit
    DialogView(const QString &path, QObject *parent=0);

    virtual
    ~DialogView();

    QDialog *dialog;

};

#endif
