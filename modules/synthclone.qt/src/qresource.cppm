export module synthclone.qt:qresource;

import synthclone.external.qt.core;
import synthclone.util;

///////////////////////////////////////////////////////////////////////////////
// synthclone::load_qresource_bytes()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    ::QByteArray
    load_qresource_bytes(const ::QString& path)
    {
        ::QResource resource(path);
        verify(
            resource.isValid(),
            "path {0:?} does not point to a valid resource",
            path.toStdString());

        ::QByteArray result(resource.uncompressedData());
        verify(
            ! result.isNull(), "path {0:?} does not point to a resource file",
            path.toStdString());

        return result;
    }

}
