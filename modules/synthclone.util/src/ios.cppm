/**
 * @file
 *
 * Contains utility functions that operate on I/O streams.
 *
 * In general, these routines should be preferred over the I/O stream class
 * methods because these functions throw exceptions with *significantly* better
 * error messages than the I/O stream class methods.
 *
 * XXX: Consider adding `std::expected` style utility functions that return
 * `std::unexpected` with `std::error_code` instead of raising exceptions.
 */

module;

#include <cerrno>

#include <synthclone/config.h>

export module synthclone.util:ios;

import std;

import synthclone.external.boost.io;
import synthclone.external.boost.numeric;

import :debug;

///////////////////////////////////////////////////////////////////////////////
// synthclone::ios_error
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Exception used by ios utility functions in this module to express I/O
     * errors.
     */

    export
    class ios_error: public std::system_error {

    public:

        /**
         * Constructs an `ios_error` instance.
         *
         * @param ev
         *   The `errno` code.
         * @param what_arg
         *   A string representing the error context.
         */

        inline
        ios_error(int ev, const char* what_arg):
            std::system_error(ev, std::generic_category(), what_arg)
        {
            // empty
        }

        /**
         * Constructs an `ios_error` instance.
         *
         * @param ev
         *   The `errno` code.
         * @param what_arg
         *   A string representing the error context.
         */

        inline
        ios_error(int ev, const std::string& what_arg):
            std::system_error(ev, std::generic_category(), what_arg)
        {
            // empty
        }

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::close()
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    constexpr
    int
    get_ios_error_code()
    {
        return errno != 0 ? errno : static_cast<int>(std::errc::io_error);
    }

    template<class F, class S>
    auto
    run_ios_operation(S& stream, F f)
    {
        boost::io::ios_exception_saver exception_state_guard(
            stream, std::ios_base::goodbit);
        boost::io::ios_iostate_saver stream_state_guard(
            stream, std::ios_base::goodbit);
        errno = 0;

        return f(stream);
    }

    [[noreturn]]
    void
    throw_ios_error(const char* what)
    {
        throw ios_error(get_ios_error_code(), what);
    }

    [[noreturn]]
    void
    throw_ios_error(const std::string& what)
    {
        throw ios_error(get_ios_error_code(), what);
    }

    /**
     * Closes the given stream.
     *
     * @param stream
     *   The stream to close.
     */

    export
    inline
    void
    close(std::ifstream& stream)
    {
        run_ios_operation(
            stream,
            [](std::ifstream& stream) {
                stream.close();
                if (stream.fail()) [[unlikely]] {
                    throw_ios_error(
                        "std::ifstream::close(): failed to close input file "
                        "stream");
                }
            });
    }

    /**
     * Closes the given stream.
     *
     * @param stream
     *   The stream to close.
     */

    export
    inline
    void
    close(std::ofstream& stream)
    {
        run_ios_operation(
            stream,
            [](std::ofstream& stream) {
                stream.close();
                if (stream.fail()) [[unlikely]] {
                    throw_ios_error(
                        "std::ofstream::close(): failed to close output file "
                        "stream");
                }
            });
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::flush()
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Flushes the given stream instance.
     *
     * @param stream
     *   The stream to flush.
     */

    export
    template<std::derived_from<std::ostream> T>
    void
    flush(T& stream)
    {
        run_ios_operation(
            stream,
            [](T& stream) {
                stream.flush();
                if (stream.bad()) [[unlikely]] {
                    throw_ios_error(
                        "std::ostream::flush(): failed to flush output "
                        "stream");
                }
            });
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::open_ifstream()
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Creates a `std::ifstream` instance that references the file at the given
     * path.
     *
     * @param path
     *   The file path.
     * @param mode
     *   The open mode.
     *
     * @return
     *   The `std::ifstream` instance.
     */

    export
    inline
    std::ifstream
    open_ifstream(
        const std::filesystem::path& path,
        std::ios_base::openmode mode = std::ios_base::in
    )
    {
        std::ifstream stream;
        run_ios_operation(
            stream,
            [&path, mode](std::ifstream& stream) {
                stream.open(path, mode);
                if (stream.fail()) [[unlikely]] {
                    throw_ios_error(
                        std::format(
                            "std::ifstream::open({0:?}, {1}): failed to open "
                            "file for input",
                            path.string(), static_cast<int>(mode)));
                }
            });
        return stream;
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::open_ofstream()
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Creates a `std::ofstream` instance that references the file at the given
     * path.
     *
     * @param path
     *   The file path.
     * @param mode
     *   The open mode.
     *
     * @return
     *   The `std::ofstream` instance.
     */

    export
    inline
    std::ofstream
    open_ofstream(
        const std::filesystem::path& path,
        std::ios_base::openmode mode = std::ios_base::out
    )
    {
        std::ofstream stream;
        run_ios_operation(
            stream,
            [&path, mode](std::ofstream& stream) {
                stream.open(path, mode);
                if (stream.fail()) [[unlikely]] {
                    throw_ios_error(
                        std::format(
                            "std::ofstream::open({0:?}, {1}): failed to open "
                            "file for output",
                            path.string(), static_cast<int>(mode)));
                }
            });
        return stream;
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::read()
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Reads data from the given stream into the given buffer.
     *
     * The function will read, at most, `buffer.size()` bytes.
     *
     * @param stream
     *   The stream to read from.
     * @param buffer
     *   The buffer to read into.
     *
     * @return
     *   The number of bytes read from the stream.  If the result is 0, then
     *   EOF has been reached.
     */

    export
    template<std::derived_from<std::istream> T>
    std::streamsize
    read(T& stream, std::span<std::byte> buffer)
    {
        auto n = buffer.size();
        verify(n != 0, "buffer size is set to 0");
        auto size = boost::numeric_cast<std::streamsize>(n);

        auto* data = buffer.data();
        verify(data != nullptr, "buffer data is set to `NULL`");

        return run_ios_operation(
            stream,
            [data, size](T& stream) -> auto {
                stream.read(reinterpret_cast<char*>(data), size);
                if (
                    stream.bad() || (stream.fail() && (! stream.eof()))
                ) [[unlikely]] {
                    throw_ios_error(
                        std::format(
                            "std::istream::read({0:x}, {1}): failed to read "
                            "from input stream",
                            reinterpret_cast<std::uintptr_t>(data), size));
                }

                // XXX: Technically, this can throw.  It's kind of terrible
                // that we can't make a strong exception guarantee for a read
                // operation if we also want to look at the amount of bytes
                // that were actually read.
                return stream.gcount();
            });
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::seek()
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Seeks to a given position in the stream.
     *
     * @param stream
     *   The stream to seek in.
     * @param offset
     *   The seek offset.
     * @param origin
     *   The seek origin.
     */

    export
    template<std::derived_from<std::istream> T>
    void
    seek(T& stream, typename T::off_type offset, std::ios_base::seekdir origin)
    {
        return run_ios_operation(
            stream,
            [offset, origin](T& stream) -> auto {
                stream.seekg(offset, origin);
                if (stream.bad() || stream.fail()) {
                    throw_ios_error(
                        std::format(
                            "std::istream::seekg({0}, {1}): failed to seek in "
                            "input stream",
                            offset, std::to_underlying(origin)));
                }
            });
    }

    /**
     * Seeks to a given position in the stream.
     *
     * @param stream
     *   The stream to seek in.
     * @param offset
     *   The seek offset.
     * @param origin
     *   The seek origin.
     */

    export
    template<std::derived_from<std::ostream> T>
    void
    seek(T& stream, typename T::off_type offset, std::ios_base::seekdir origin)
    {
        return run_ios_operation(
            stream,
            [offset, origin](T& stream) -> auto {
                stream.seekp(offset, origin);
                if (stream.bad() || stream.fail()) {
                    throw_ios_error(
                        std::format(
                            "std::istream::seekp({0}, {1}): failed to seek in "
                            "input stream",
                            offset, std::to_underlying(origin)));
                }
            });
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::tell()
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Gets the current position in the stream.
     *
     * @param stream
     *   The stream to get the position for.
     *
     * @return
     *   The stream position, or `std::nullopt` if seeking is not supported by
     *   the given stream.
     */

    export
    template<std::derived_from<std::istream> T>
    typename T::pos_type
    tell(T& stream)
    {
        return run_ios_operation(
            stream,
            [](T& stream) -> auto {
                auto position = stream.tellg();
                if (position == typename T::pos_type(-1)) {
                    throw_ios_error(
                        "std::istream::tellg(): failed to get input stream "
                        "position");
                }
                return position;
            });
    }

    /**
     * Gets the current position in the stream.
     *
     * @param stream
     *   The stream to get the position for.
     *
     * @return
     *   The stream position, or `std::nullopt` if seeking is not supported by
     *   the given stream.
     */

    export
    template<std::derived_from<std::ostream> T>
    typename T::pos_type
    tell(T& stream)
    {
        return run_ios_operation(
            stream,
            [](T& stream) -> auto {
                auto position = stream.tellp();
                if (position == typename T::pos_type(-1)) {
                    throw_ios_error(
                        "std::istream::tellp(): failed to get output stream "
                        "position");
                }
                return position;
            });
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::write()
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Writes data from the given buffer to the given stream.
     *
     * @param stream
     *   The stream to write to.
     * @param buffer
     *   The buffer to read from.
     */

    export
    template<std::derived_from<std::ostream> T>
    void
    write(T& stream, std::span<const std::byte> buffer)
    {
        auto n = buffer.size();
        verify(n != 0, "buffer size is set to 0");
        auto size = boost::numeric_cast<std::streamsize>(n);

        const auto* data = buffer.data();
        verify(data != nullptr, "buffer data is set to `NULL`");

        run_ios_operation(
            stream,
            [data, size](T& stream) {
                stream.write(reinterpret_cast<const char*>(data), size);
                if (stream.bad()) [[unlikely]] {
                    throw_ios_error(
                        std::format(
                            "std::ostream::write({0:x}, {1}): failed to write "
                            "to output stream",
                            reinterpret_cast<std::uintptr_t>(data), size));
                }
            });
    }

}
