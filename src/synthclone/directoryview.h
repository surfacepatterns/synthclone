#ifndef __DIRECTORYVIEW_H__
#define __DIRECTORYVIEW_H__

#include <QtGui/QFileDialog>

#include <synthclone/view.h>

class DirectoryView: public synthclone::View {

    Q_OBJECT

public:

    enum Mode {
        MODE_LOADING,
        MODE_SAVING
    };

    explicit
    DirectoryView(QObject *parent=0);

    ~DirectoryView();

    void
    setMode(Mode mode);

    void
    setPath(const QString &path);

    void
    setTitle(const QString &title);

signals:

    void
    pathSelected(const QString &path);

private slots:

    void
    handleDialogAccept();

private:

    QFileDialog *dialog;

};

#endif
