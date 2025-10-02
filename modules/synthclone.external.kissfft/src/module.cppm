module;

#include <kissfft/kiss_fftr.h>

// Working around macros for fun and prof ... err, more fun.

using kiss_fft_scalar_ = kiss_fft_scalar;

inline void
kiss_fftr_free_(void* ptr)
noexcept
{
    kiss_fftr_free(ptr);
}

#undef kiss_fft_scalar
#undef kiss_fftr_free

export module synthclone.external.kissfft;

import std;

static_assert(std::same_as<float, kiss_fft_scalar_>);

export {

    inline void
    kiss_fftr_free(void* ptr)
    noexcept
    {
        kiss_fftr_free_(ptr);
    }

    using kiss_fft_scalar = kiss_fft_scalar_;

    using ::kiss_fft_cpx;
    using ::kiss_fftr_cfg;

    using ::kiss_fftr;
    using ::kiss_fftr_alloc;

}
