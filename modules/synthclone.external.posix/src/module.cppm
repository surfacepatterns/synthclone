module;

#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>

export module synthclone.external.posix;

export {

    using ::mode_t;

    using ::close;
    using ::mkdtemp;
    using ::mkstemp;
    using ::rmdir;
    using ::unlink;

}
