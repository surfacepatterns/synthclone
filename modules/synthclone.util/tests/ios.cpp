#include <cerrno>

#include <boost/test/unit_test.hpp>

import std;

import synthclone.test;
import synthclone.util;

BOOST_AUTO_TEST_SUITE(ios)

BOOST_AUTO_TEST_CASE(basic_fstream_ops)
{
    synthclone::temporary_directory directory;
    auto path = directory.path() / "foo";
    std::array<char, 3> write_buffer {'f', 'o', 'o'};

    {
        auto stream = synthclone::open_ofstream(path);
        BOOST_REQUIRE(stream.is_open());

        synthclone::write(stream, std::as_bytes(std::span(write_buffer)));
        BOOST_REQUIRE(stream.is_open());

        synthclone::close(stream);
        BOOST_REQUIRE(! stream.is_open());
    }

    synthclone::verify_eq(3, std::filesystem::file_size(path));

    auto stream = synthclone::open_ifstream(path);
    BOOST_REQUIRE(stream.is_open());

    std::array<char, 3> read_buffer;
    synthclone::read(stream, std::as_writable_bytes(std::span(read_buffer)));
    BOOST_REQUIRE(stream.is_open());

    synthclone::close(stream);
    BOOST_REQUIRE(! stream.is_open());
    BOOST_REQUIRE(std::ranges::equal(write_buffer, read_buffer));
}

BOOST_AUTO_TEST_CASE(basic_istream_ops)
{
    const std::string s("1, 2, 3, 4, I declare a thumb war.");
    std::istringstream stream(s);

    synthclone::verify_eq(0, synthclone::tell(stream));

    std::array<std::byte, 16> buffer;
    auto count = synthclone::read(stream, buffer);
    synthclone::verify_eq(16, count);
    BOOST_REQUIRE(
        std::ranges::equal(
            std::as_bytes(std::span(s)).first(16), std::span(buffer)));
    synthclone::verify_eq(16, synthclone::tell(stream));

    synthclone::seek(stream, 16, std::ios_base::cur);
    synthclone::verify_eq(32, synthclone::tell(stream));

    count = synthclone::read(stream, buffer);
    synthclone::verify_eq(2, count);
    BOOST_REQUIRE(
        std::ranges::equal(
            std::as_bytes(std::span(s)).last(2), std::span(buffer).first(2)));
    synthclone::verify_eq(34, synthclone::tell(stream));

    synthclone::seek(stream, 16, std::ios_base::beg);
    synthclone::verify_eq(16, synthclone::tell(stream));

    count = synthclone::read(stream, buffer);
    synthclone::verify_eq(16, count);
    BOOST_REQUIRE(
        std::ranges::equal(
            std::as_bytes(std::span(s)).subspan(16, 16), std::span(buffer)));
    synthclone::verify_eq(32, synthclone::tell(stream));

    synthclone::seek(stream, 0, std::ios_base::end);
    synthclone::verify_eq(34, synthclone::tell(stream));
}

BOOST_AUTO_TEST_CASE(basic_ostream_ops)
{
    std::ostringstream stream;

    synthclone::verify_eq(0, synthclone::tell(stream));

    synthclone::write(
        stream, std::as_bytes(std::span(std::string_view("foo"))));
    synthclone::verify_eq("foo", stream.str());
    synthclone::verify_eq(3, synthclone::tell(stream));

    synthclone::write(
        stream, std::as_bytes(std::span(std::string_view("bar"))));
    synthclone::verify_eq("foobar", stream.str());
    synthclone::verify_eq(6, synthclone::tell(stream));

    synthclone::seek(stream, 0, std::ios_base::beg);
    synthclone::verify_eq("foobar", stream.str());
    synthclone::verify_eq(0, synthclone::tell(stream));

    synthclone::write(
        stream, std::as_bytes(std::span(std::string_view("baz"))));
    synthclone::verify_eq("bazbar", stream.str());
    synthclone::verify_eq(3, synthclone::tell(stream));

    synthclone::seek(stream, 0, std::ios_base::end);
    synthclone::verify_eq("bazbar", stream.str());
    synthclone::verify_eq(6, synthclone::tell(stream));

    synthclone::flush(stream);
    synthclone::verify_eq("bazbar", stream.str());
    synthclone::verify_eq(6, synthclone::tell(stream));
}

BOOST_AUTO_TEST_CASE(close_errors)
{
    {
        std::ifstream stream;
        BOOST_CHECK_THROW(synthclone::close(stream), synthclone::ios_error);
    }

    {
        std::ofstream stream;
        BOOST_CHECK_THROW(synthclone::close(stream), synthclone::ios_error);
    }
}

BOOST_AUTO_TEST_CASE(flush_errors)
{

    class streambuf final: public std::streambuf {

    public:

        explicit
        streambuf(int code = 0):
            std::streambuf(),
            code_(code)
        {
            // empty
        }

    private:

        int
        sync() override final
        {
            errno = code_;
            return -1;
        }

        int code_;

    };

    {
        streambuf buffer;
        std::ostream stream(&buffer);
        BOOST_CHECK_EXCEPTION(
            synthclone::flush(stream), synthclone::ios_error,
            [](const synthclone::ios_error& e) {
                return e.code().value() ==
                    static_cast<int>(std::errc::io_error);
            });
    }

    {
        streambuf buffer(static_cast<int>(std::errc::network_down));
        std::ostream stream(&buffer);
        BOOST_CHECK_EXCEPTION(
            synthclone::flush(stream), synthclone::ios_error,
            [](const synthclone::ios_error& e) {
                return e.code().value() ==
                    static_cast<int>(std::errc::network_down);
            });
    }
}

BOOST_AUTO_TEST_CASE(open_ifstream_errors)
{
    BOOST_CHECK_THROW(
        synthclone::open_ifstream("/this/path/does/not/exist"),
        synthclone::ios_error);
}

BOOST_AUTO_TEST_CASE(open_ofstream_errors)
{
    BOOST_CHECK_THROW(
        synthclone::open_ofstream("/this/path/does/not/exist"),
        synthclone::ios_error);
}

BOOST_AUTO_TEST_CASE(read_eof)
{

    class streambuf final: public std::streambuf {

    private:

        std::char_traits<char>::int_type
        underflow() override final
        {
            return std::char_traits<char>::eof();
        }

    };

    streambuf stream_buffer;
    std::istream stream(&stream_buffer);

    std::array<std::byte, 1> buffer;
    auto count = synthclone::read(stream, buffer);
    synthclone::verify_eq(0, count);
}

BOOST_AUTO_TEST_CASE(read_errors)
{

    class streambuf final: public std::streambuf {

    public:

        explicit
        streambuf(int code = 0):
            std::streambuf(),
            code_(code)
        {
            // empty
        }

    private:

        std::char_traits<char>::int_type
        underflow() override final
        {
            errno = code_;
            throw std::ios_base::failure("foo");
        }

        int code_;

    };

    std::array<std::byte, 1> buffer;

    {
        const std::string s("foo");
        std::istringstream stream(s);

        std::span span(buffer.data(), 0);
        BOOST_CHECK_THROW(
            synthclone::read(stream, span), synthclone::verification_error);

        span = std::span<std::byte>(static_cast<std::byte*>(nullptr), 2);
        BOOST_CHECK_THROW(
            synthclone::read(stream, span), synthclone::verification_error);
    }

    std::span span(buffer);

    {
        streambuf buffer;
        std::istream stream(&buffer);
        BOOST_CHECK_EXCEPTION(
            synthclone::read(stream, span), synthclone::ios_error,
            [](const synthclone::ios_error& e) {
                return e.code().value() ==
                    static_cast<int>(std::errc::io_error);
            });
    }

    {
        streambuf buffer(static_cast<int>(std::errc::network_down));
        std::istream stream(&buffer);
        BOOST_CHECK_EXCEPTION(
            synthclone::read(stream, span), synthclone::ios_error,
            [](const synthclone::ios_error& e) {
                return e.code().value() ==
                    static_cast<int>(std::errc::network_down);
            });
    }

}

BOOST_AUTO_TEST_CASE(seek_errors)
{

    class streambuf final: public std::streambuf {

    public:

        explicit
        streambuf(int code = 0):
            std::streambuf(),
            code_(code)
        {
            // empty
        }

    private:

        pos_type
        seekoff(
            off_type off,
            std::ios_base::seekdir dir,
            std::ios_base::openmode which =
                std::ios_base::in | std::ios_base::out
        ) override final
        {
            errno = code_;
            return pos_type(off_type(-1));
        }

        int code_;

    };

    {
        streambuf buffer;
        std::istream stream(&buffer);
        BOOST_CHECK_EXCEPTION(
            synthclone::seek(stream, 1, std::ios_base::beg),
            synthclone::ios_error,
            [](const synthclone::ios_error& e) {
                return e.code().value() ==
                    static_cast<int>(std::errc::io_error);
            });
    }

    {
        streambuf buffer(static_cast<int>(std::errc::network_down));
        std::istream stream(&buffer);
        BOOST_CHECK_EXCEPTION(
            synthclone::seek(stream, 1, std::ios_base::beg),
            synthclone::ios_error,
            [](const synthclone::ios_error& e) {
                return e.code().value() ==
                    static_cast<int>(std::errc::network_down);
            });
    }

    {
        streambuf buffer;
        std::ostream stream(&buffer);
        BOOST_CHECK_EXCEPTION(
            synthclone::seek(stream, 1, std::ios_base::beg),
            synthclone::ios_error,
            [](const synthclone::ios_error& e) {
                return e.code().value() ==
                    static_cast<int>(std::errc::io_error);
            });
    }

    {
        streambuf buffer(static_cast<int>(std::errc::network_down));
        std::ostream stream(&buffer);
        BOOST_CHECK_EXCEPTION(
            synthclone::seek(stream, 1, std::ios_base::beg),
            synthclone::ios_error,
            [](const synthclone::ios_error& e) {
                return e.code().value() ==
                    static_cast<int>(std::errc::network_down);
            });
    }

}

BOOST_AUTO_TEST_CASE(tell_errors)
{

    class streambuf final: public std::streambuf {

    public:

        explicit
        streambuf(int code = 0):
            std::streambuf(),
            code_(code)
        {
            // empty
        }

    private:

        pos_type
        seekoff(
            off_type off,
            std::ios_base::seekdir dir,
            std::ios_base::openmode which =
                std::ios_base::in | std::ios_base::out
        ) override final
        {
            errno = code_;
            return pos_type(off_type(-1));
        }

        int code_;

    };

    {
        streambuf buffer;
        std::istream stream(&buffer);
        BOOST_CHECK_EXCEPTION(
            synthclone::tell(stream), synthclone::ios_error,
            [](const synthclone::ios_error& e) {
                return e.code().value() ==
                    static_cast<int>(std::errc::io_error);
            });
    }

    {
        streambuf buffer(static_cast<int>(std::errc::network_down));
        std::istream stream(&buffer);
        BOOST_CHECK_EXCEPTION(
            synthclone::tell(stream), synthclone::ios_error,
            [](const synthclone::ios_error& e) {
                return e.code().value() ==
                    static_cast<int>(std::errc::network_down);
            });
    }

    {
        streambuf buffer;
        std::ostream stream(&buffer);
        BOOST_CHECK_EXCEPTION(
            synthclone::tell(stream), synthclone::ios_error,
            [](const synthclone::ios_error& e) {
                return e.code().value() ==
                    static_cast<int>(std::errc::io_error);
            });
    }

    {
        streambuf buffer(static_cast<int>(std::errc::network_down));
        std::ostream stream(&buffer);
        BOOST_CHECK_EXCEPTION(
            synthclone::tell(stream), synthclone::ios_error,
            [](const synthclone::ios_error& e) {
                return e.code().value() ==
                    static_cast<int>(std::errc::network_down);
            });
    }

}

BOOST_AUTO_TEST_CASE(write_errors)
{

    class streambuf final: public std::streambuf {

    public:

        explicit
        streambuf(int code = 0):
            std::streambuf(),
            code_(code)
        {
            // empty
        }

    private:

        std::char_traits<char>::int_type
        overflow(int_type ch = std::char_traits<char>::eof()) override final
        {
            errno = code_;
            throw std::ios_base::failure("foo");
        }

        int code_;

    };

    std::array<const std::byte, 1> buffer { std::byte(0) };

    {
        std::ostringstream stream;

        std::span span(buffer.data(), 0);
        BOOST_CHECK_THROW(
            synthclone::write(stream, span), synthclone::verification_error);

        span = std::span<std::byte>(static_cast<std::byte*>(nullptr), 2);
        BOOST_CHECK_THROW(
            synthclone::write(stream, span), synthclone::verification_error);
    }

    std::span span(buffer);

    {
        streambuf buffer;
        std::ostream stream(&buffer);
        BOOST_CHECK_EXCEPTION(
            synthclone::write(stream, span), synthclone::ios_error,
            [](const synthclone::ios_error& e) {
                return e.code().value() ==
                    static_cast<int>(std::errc::io_error);
            });
    }

    {
        streambuf buffer(static_cast<int>(std::errc::network_down));
        std::ostream stream(&buffer);
        BOOST_CHECK_EXCEPTION(
            synthclone::write(stream, span), synthclone::ios_error,
            [](const synthclone::ios_error& e) {
                return e.code().value() ==
                    static_cast<int>(std::errc::network_down);
            });
    }

}

BOOST_AUTO_TEST_SUITE_END()
