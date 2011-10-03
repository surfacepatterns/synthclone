#ifndef __ARCHIVEWRITER_H__
#define __ARCHIVEWRITER_H__

#include <archive.h>

#include <synthclone/sample.h>

class ArchiveWriter: public QObject {

    Q_OBJECT

public:

    ArchiveWriter(const QString &path, const QString &kitName,
                  QObject *parent=0);

    ~ArchiveWriter();

    void
    addConfiguration(const QString &configuration);

    void
    addSample(const QString &fileName, const synthclone::Sample &sample);

    void
    close();

private:

    void
    writeEntry(const QString &fileName, int64_t size);

    archive *arch;
    bool closed;
    QString kitName;

};

#endif
