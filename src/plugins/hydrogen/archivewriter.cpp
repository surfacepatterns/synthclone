#include <cassert>
#include <stdexcept>

#include <archive_entry.h>

#include <QtCore/QDebug>
#include <QtCore/QFileInfo>

#include <synthclone/error.h>

#include "archivewriter.h"

ArchiveWriter::ArchiveWriter(const QString &path, const QString &kitName,
                             QObject *parent):
    QObject(parent)
{
    arch = archive_write_new();
    if (! arch) {
        throw std::bad_alloc();
    }
    try {
        if (archive_write_set_compression_gzip(arch) != ARCHIVE_OK) {
            throw synthclone::Error(archive_error_string(arch));
        }
        if (archive_write_set_format_pax_restricted(arch) != ARCHIVE_OK) {
            throw synthclone::Error(archive_error_string(arch));
        }
        QByteArray pathBytes = path.toLocal8Bit();
        if (archive_write_open_filename(arch, pathBytes.constData()) !=
            ARCHIVE_OK) {
            throw synthclone::Error(archive_error_string(arch));
        }
    } catch (...) {
        archive_write_finish(arch);
        throw;
    }
    closed = false;
    this->kitName = kitName;
}

ArchiveWriter::~ArchiveWriter()
{
    if (! closed) {
        try {
            close();
        } catch (synthclone::Error &e) {
            qWarning() << tr("Error in ArchiveWriter destructor: %1").
                arg(e.getMessage());
        }
    }
    archive_write_finish(arch);
}

void
ArchiveWriter::addConfiguration(const QString &configuration)
{
    int64_t size = static_cast<int64_t>(configuration.count());
    writeEntry("drumkit.xml", configuration.count());
    QByteArray confBytes = configuration.toLocal8Bit();
    ssize_t n = archive_write_data(arch, confBytes.constData(), size);
    if (n == -1) {
        throw synthclone::Error(archive_error_string(arch));
    }
    assert(n == size);
}

void
ArchiveWriter::addSample(const QString &fileName,
                         const synthclone::Sample &sample)
{
    QString path = sample.getPath();
    QFileInfo info(path);
    writeEntry(fileName, info.size());
    QFile file(path);
    if (! file.open(QFile::ReadOnly)) {
        QString message = tr("could not open '%1': %2").
            arg(info.absoluteFilePath(), file.errorString());
        throw synthclone::Error(message);
    }
    for (;;) {
        QByteArray data = file.read(8192);
        if (data.isEmpty()) {
            break;
        }
        int length = data.count();
        ssize_t n = archive_write_data(arch, data.constData(), length);
        if (n == -1) {
            throw synthclone::Error(archive_error_string(arch));
        }
        assert(n == length);
    }
}

void
ArchiveWriter::close()
{
    assert(! closed);
    if (archive_write_close(arch) != ARCHIVE_OK) {
        throw synthclone::Error(archive_error_string(arch));
    }
    closed = true;
}

void
ArchiveWriter::writeEntry(const QString &fileName, int64_t size)
{
    assert(! fileName.isEmpty());
    assert(size > 0);
    struct archive_entry *entry = archive_entry_new();
    if (! entry) {
        throw std::bad_alloc();
    }
    QByteArray path = QString("%1/%2").arg(kitName, fileName).toLocal8Bit();
    archive_entry_set_pathname(entry, path.constData());
    archive_entry_set_size(entry, size);
    archive_entry_set_filetype(entry, AE_IFREG);
    archive_entry_set_perm(entry, 0644);
    int result = archive_write_header(arch, entry);
    archive_entry_free(entry);
    if (result != ARCHIVE_OK) {
        throw synthclone::Error(archive_error_string(arch));
    }
}
