module;

#include <stdlib.h>
#include <unistd.h>

export module synthclone.external.posix;

export {

    using ::close;
    using ::mkdtemp;
    using ::mkstemp;
    using ::rmdir;
    using ::unlink;

}
