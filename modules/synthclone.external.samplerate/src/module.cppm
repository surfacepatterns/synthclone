module;

#include <samplerate.h>

export module synthclone.external.samplerate;

export {

    using ::SRC_DATA;
    using ::SRC_SINC_BEST_QUALITY;
    using ::SRC_STATE;

    using ::src_delete;
    using ::src_is_valid_ratio;
    using ::src_new;
    using ::src_process;
    using ::src_reset;
    using ::src_strerror;

}
