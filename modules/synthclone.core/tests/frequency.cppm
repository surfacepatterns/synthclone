module;

#include <cerrno>

#include <boost/test/unit_test.hpp>

#include <kissfft/kiss_fftr.h>

export module synthclone.core.tests:frequency;

import std;

import synthclone.core;
import synthclone.util;

import :audio;

///////////////////////////////////////////////////////////////////////////////
// synthclone::frequency_buffer_size
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    template<std::size_t N>
    constexpr inline
    std::size_t
    frequency_buffer_size = (N / 2) + 1;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::frequency_buffer
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    template<std::size_t N>
    using frequency_buffer = fixed_buffer<double, N>;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::frequency_transformer
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    static_assert(std::same_as<audio_sample, kiss_fft_scalar>);

    struct kissfft_config_destroyer final {

        void
        operator()(::kiss_fftr_cfg ptr)
        {
            ::kiss_fftr_free(ptr);
        }

    };

    using kissfft_config_ptr = std::unique_ptr<
        std::remove_pointer_t<::kiss_fftr_cfg>,
        kissfft_config_destroyer
    >;

    template<std::size_t N>
    kissfft_config_ptr
    make_kissfft_config()
    {
        errno = 0;
        auto ptr = ::kiss_fftr_alloc(N, false, nullptr, nullptr);
        if (ptr == nullptr) [[unlikely]] {
            throw std::system_error(
                errno, std::generic_category(), "kiss_fftr_alloc()");
        }
        return kissfft_config_ptr(ptr);
    }

    export
    template<std::size_t N>
    class frequency_transformer final: private noncopyable {

        static_assert(
            (N != 0) && ((N % 2) == 0), "`N` must be even and non-zero");
        static_assert(N != std::dynamic_extent, "`N` must be a static size");

    public:

        inline
        frequency_transformer():
            impl_(make_kissfft_config<N>())
        {
            // empty
        }

        void
        transform(
            std::span<const audio_sample, N> audio_span,
            std::span<double, frequency_buffer_size<N>> frequency_span
        )
        {
            constexpr double scale = 2.0 / static_cast<double>(N);

            ::kiss_fftr(
                impl_.get(), audio_span.data(), interim_buffer_.data());

            // The DC component doesn't have an imaginary element.
            frequency_span[0] = interim_buffer_[0].r * scale;

            std::ranges::for_each(
                std::views::iota(1U, frequency_buffer_size<N>),
                [this, frequency_span](std::size_t i) {
                    const auto c = interim_buffer_[i];
                    const auto imag = static_cast<double>(c.i);
                    const auto real = static_cast<double>(c.r);
                    frequency_span[i] =
                        std::sqrt((real * real) + (imag * imag)) * scale;
                });
        }

    private:

        std::array<::kiss_fft_cpx, frequency_buffer_size<N>> interim_buffer_;
        kissfft_config_ptr impl_;

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::get_frequency_transformer()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    // There's some cost to creating a `frequency_transformer` instance, so we
    // cache `frequency_tranformer` instances for commonly used FFT sizes.

    export
    template<std::size_t N>
    frequency_transformer<N>&
    get_frequency_transformer()
    {
        static thread_local frequency_transformer<N> transformer;
        return transformer;
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::load_frequencies()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    template<std::size_t ChannelCount, std::size_t ChannelIndex, std::size_t N>
    requires (N != std::dynamic_extent)
    frequency_buffer<frequency_buffer_size<N / ChannelCount>>
    load_channel_frequencies(std::span<const audio_sample, N> audio_span)
    {
        BOOST_TEST_INFO_SCOPE(
            std::format(
                "synthclone::load_channel_frequencies<{0}, {1}, {2}>("
                "audio_sample_buffer<{2}>)",
                ChannelCount, ChannelIndex, N));

        constexpr std::size_t sample_count = N / ChannelCount;
        constexpr std::size_t frequency_count =
            frequency_buffer_size<sample_count>;

        using frequency_span = std::span<double, frequency_count>;

        auto& transformer = get_frequency_transformer<sample_count>();
        frequency_buffer<frequency_count> frequencies;

        if constexpr(ChannelCount == 1) {
            transformer.transform(audio_span, frequency_span(frequencies));
        } else {
            audio_sample_buffer<sample_count> channel_buffer;
            std::ranges::copy(
                std::ranges::stride_view(
                    std::ranges::subrange(
                        audio_span.begin() + ChannelIndex,
                        audio_span.end() + ChannelIndex),
                    ChannelCount),
                channel_buffer.begin());
            transformer.transform(
                std::span<const audio_sample, sample_count>(channel_buffer),
                frequency_span(frequencies));
        }

        return frequencies;
    }

    template<std::size_t N, std::size_t... ChannelIndices>
    requires (N != std::dynamic_extent)
    auto
    load_frequencies(
        std::span<const audio_sample, N> audio_span,
        std::index_sequence<ChannelIndices...>
    )
    {
        BOOST_TEST_INFO_SCOPE(
            std::format(
                "synthclone::load_frequencies<{0}, ...>("
                "audio_sample_buffer<{0}>)",
                N));

        constexpr std::size_t channel_count = sizeof...(ChannelIndices);

        return std::make_tuple(
            load_channel_frequencies<
                channel_count,
                ChannelIndices
            >(audio_span)...
        );
    }

    export
    template<std::size_t ChannelCount, std::size_t N>
    requires (N != std::dynamic_extent)
    auto
    load_frequencies(std::span<const audio_sample, N> audio_span)
    {
        BOOST_TEST_INFO_SCOPE(
            std::format(
                "synthclone::load_frequencies<{0}, {1}>("
                "std::span<const audio_sample, {1}>)",
                ChannelCount, N));

        return load_frequencies(
            audio_span, std::make_index_sequence<ChannelCount>());
    }

    export
    template<std::size_t ChannelCount, std::size_t N>
    auto
    load_frequencies(const audio_sample_buffer<N>& buffer)
    {
        BOOST_TEST_INFO_SCOPE(
            std::format(
                "synthclone::load_frequencies<{0}, {1}>("
                "audio_sample_buffer<{1}>)",
                ChannelCount, N));

        return load_frequencies<ChannelCount>(
            std::span<const audio_sample, N>(buffer));
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::verify_frequencies()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    template<audio_sample_rate::scalar_type SampleRate, std::size_t N>
    constexpr
    double
    calculate_bucket_frequency(std::size_t bucket_index)
    {
        constexpr double nyquist = static_cast<double>(SampleRate / 2);
        constexpr double divisor = static_cast<double>(N - 1);

        return (static_cast<double>(bucket_index) / divisor) * nyquist;
    }

    template<
        double PeakThreshold,
        std::size_t ChannelIndex,
        std::size_t N
    >
    void
    verify_channel_frequencies(
        const frequency_buffer<N>& expected_frequencies,
        const frequency_buffer<N>& actual_frequencies
    )
    {
        BOOST_TEST_INFO_SCOPE(
            std::format(
                "synthclone::verify_channel_frequencies<{0}, {1}, {2}>("
                "const frequency_buffer<{2}>&, const frequency_buffer<{2}>&)",
                PeakThreshold, ChannelIndex, N));

        verify_frequencies<PeakThreshold>(
            expected_frequencies, actual_frequencies);
    }

    template<
        double PeakThreshold,
        std::size_t N,
        std::same_as<frequency_buffer<N>>... T,
        std::same_as<frequency_buffer<N>>... U,
        std::size_t... ChannelIndices
    >
    requires(
        (sizeof...(T) == sizeof...(U)) &&
        ((sizeof...(T) + 1) == sizeof...(ChannelIndices))
    )
    void
    verify_frequencies(
        const std::tuple<frequency_buffer<N>, T...>& expected_frequencies,
        const std::tuple<frequency_buffer<N>, U...>& actual_frequencies,
        std::index_sequence<ChannelIndices...>
    )
    {
        static_assert(N > 1);

        BOOST_TEST_INFO_SCOPE(
            std::format(
                "synthclone::verify_frequencies<{0}, ...>("
                "const std::tuple<frequency_buffer<{1}>, ...>&, "
                "const std::tuple<frequency_buffer<{1}>, ...>&, "
                "std::index_sequence<...>)",
                PeakThreshold, N));

        (
            verify_channel_frequencies<PeakThreshold, ChannelIndices>(
                std::get<ChannelIndices>(expected_frequencies),
                std::get<ChannelIndices>(actual_frequencies)),
            ...
        );
    }

    export
    template<double PeakThreshold, std::size_t N>
    void
    verify_frequencies(
        const frequency_buffer<N>& expected_frequencies,
        const frequency_buffer<N>& actual_frequencies
    )
    {
        static_assert(N > 1);

        BOOST_TEST_INFO_SCOPE(
            std::format(
                "synthclone::verify_frequencies<{0}, {1}>("
                "const frequency_buffer<{1}>&, const frequency_buffer<{1}>&)",
                PeakThreshold, N));

        constexpr std::size_t sample_rate = (N - 1) * 2;

        double peak_actual_power = actual_frequencies[1];
        double peak_expected_power = expected_frequencies[1];
        double peak_noise_power = std::abs(
            peak_actual_power - peak_expected_power);

        std::size_t peak_actual_bucket = 1;
        std::size_t peak_noise_bucket = 1;
        std::size_t peak_expected_bucket = 1;

        std::ranges::for_each(
            std::ranges::views::iota(2U, N),
            [
                &actual_frequencies, &expected_frequencies,
                &peak_actual_bucket, &peak_actual_power,
                &peak_noise_bucket, &peak_noise_power,
                &peak_expected_bucket, &peak_expected_power
            ] (const std::size_t index) {
                const auto actual_magnitude = actual_frequencies[index];
                const auto expected_magnitude = expected_frequencies[index];

                const auto n = std::abs(
                    actual_magnitude - expected_magnitude);
                if (n > peak_noise_power) {
                    peak_noise_power = n;
                    peak_noise_bucket = index;
                }

                if (actual_magnitude > peak_actual_power) {
                    peak_actual_power = actual_magnitude;
                    peak_actual_bucket = index;
                }
                if (expected_magnitude > peak_expected_power) {
                    peak_expected_power = expected_magnitude;
                    peak_expected_bucket = index;
                }
            });

        // trace(
        //     "peak actual: {0} ({1} Hz), peak expected: {2} ({3} Hz), "
        //     "peak noise: {4} ({5} Hz)",
        //     peak_actual_power,
        //     calculate_bucket_frequency<sample_rate, N>(peak_actual_bucket),
        //     peak_expected_power,
        //     calculate_bucket_frequency<sample_rate, N>(peak_expected_bucket),
        //     peak_noise_power,
        //     calculate_bucket_frequency<sample_rate, N>(peak_noise_bucket));

        BOOST_CHECK_MESSAGE(
            peak_noise_power <= PeakThreshold,
            (
                std::format(
                    "actual audio check failed: peak noise "
                    "bucket [{0} ({1} dBFS) @ {2} Hz] exceeds acceptable "
                    "threshold [{3} ({4} dBFS)] -- peak expected bucket "
                    "[{5} ({6} dBFS) @ {7} Hz], peak actual bucket "
                    "[{8} ({9} dBFS) @ {10} Hz]",
                    peak_noise_power, to_dbfs(peak_noise_power),
                    calculate_bucket_frequency<sample_rate, N>(
                        peak_noise_bucket),
                    PeakThreshold, to_dbfs(PeakThreshold),
                    peak_expected_power, to_dbfs(peak_expected_power),
                    calculate_bucket_frequency<sample_rate, N>(
                        peak_expected_bucket),
                    peak_actual_power, to_dbfs(peak_actual_power),
                    calculate_bucket_frequency<sample_rate, N>(
                        peak_actual_bucket))
            ));
    }

    export
    template<
        double PeakThreshold,
        std::size_t N,
        std::same_as<frequency_buffer<N>>... T,
        std::same_as<frequency_buffer<N>>... U
    >
    requires(sizeof...(T) == sizeof...(U))
    void
    verify_frequencies(
        const std::tuple<frequency_buffer<N>, T...>& expected_frequencies,
        const std::tuple<frequency_buffer<N>, U...>& actual_frequencies
    )
    {
        static_assert(N > 1);

        BOOST_TEST_INFO_SCOPE(
            std::format(
                "synthclone::verify_frequencies<{0}, ...>("
                "const std::tuple<frequency_buffer<{1}>, ...>&, "
                "const std::tuple<frequency_buffer<{1}>, ...>&)",
                PeakThreshold, N));

        verify_frequencies<PeakThreshold>(
            expected_frequencies, actual_frequencies,
            std::make_index_sequence<sizeof...(T) + 1>());
    }

}
