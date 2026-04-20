/**
 * @file
 *
 * Specifies aspects of `synthclone` sessions that are available to plugins.
 */

module;

#include <synthclone/config.h>

export module synthclone.core:session;

import synthclone.util;

import :audio;

///////////////////////////////////////////////////////////////////////////////
// synthclone::session_info
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Contains information about the session.
     */

    export
    class session_info final: private nonmovable {

    public:

        /**
         * Constructs a `session_info` instance.
         *
         * @param audio_traits
         *   The audio traits used for samples stored in this session.
         */

        explicit
        session_info(const audio_traits& audio_traits) noexcept:
            audio_traits_(audio_traits)
        {
            // empty
        }

        /**
         * Gets the audio traits used for samples stored in this session.
         *
         * @return
         *   The audio traits.
         */

        constexpr
        const audio_traits&
        audio_traits() const noexcept
        {
            return audio_traits_;
        }

    private:

        class audio_traits audio_traits_;

    };

}
