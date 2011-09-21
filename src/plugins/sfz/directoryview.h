#ifndef __DIRECTORYVIEW_H__
#define __DIRECTORYVIEW_H__

#include <QtGui/QFileDialog>

#include <synthclone/view.h>

class DirectoryView: public synthclone::View {

    Q_OBJECT

public:

    explicit
    DirectoryView(QObject *parent=0);

    ~DirectoryView();

    void
    setPath(const QString &path);

signals:

    void
    pathSelected(const QString &path);

private slots:

    void
    handleDialogAccept();

};

#endif
