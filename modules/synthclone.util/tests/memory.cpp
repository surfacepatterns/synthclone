#include <boost/test/unit_test.hpp>

import std;

import synthclone.util;

namespace {

    template<class T>
    void
    verify_non_pointer()
    {
        BOOST_CHECK(! synthclone::pointer<T>);
    }

    template<class T>
    void
    verify_pointer()
    {
        using traits = std::pointer_traits<std::remove_cv_t<T>>;

        BOOST_CHECK(synthclone::pointer<T>);

        BOOST_CHECK(
            (
                std::same_as<
                    synthclone::pointer_address_t<T>,
                    typename traits::element_type*
                >
            ));
        BOOST_CHECK(
            (
                std::same_as<
                    synthclone::pointer_difference_t<T>,
                    typename traits::difference_type
                >
            ));
        BOOST_CHECK(
            (
                std::same_as<
                    synthclone::pointer_element_t<T>,
                    typename traits::element_type
                >
            ));
    }

}

BOOST_AUTO_TEST_SUITE(memory)

BOOST_AUTO_TEST_CASE(pointer)
{
    verify_pointer<int*>();
    verify_pointer<char*>();
    verify_pointer<const int*>();
    verify_pointer<const char*>();
    verify_pointer<volatile int*>();
    verify_pointer<volatile char*>();
    verify_pointer<const volatile int*>();
    verify_pointer<const volatile char*>();
    verify_pointer<int* const>();
    verify_pointer<char* const>();
    verify_pointer<int* volatile>();
    verify_pointer<char* volatile>();
    verify_pointer<int* const volatile>();
    verify_pointer<char* const volatile>();
    verify_pointer<std::unique_ptr<int>>();
    verify_pointer<std::shared_ptr<float>>();
    verify_pointer<const std::unique_ptr<int>>();
    verify_pointer<const std::shared_ptr<float>>();
    verify_pointer<void*>();

    verify_non_pointer<int>();
    verify_non_pointer<char>();
    verify_non_pointer<std::unique_ptr<int>&>();
    verify_non_pointer<std::shared_ptr<float>&>();
    verify_non_pointer<std::unique_ptr<int>&&>();
    verify_non_pointer<std::shared_ptr<float>&&>();
    verify_non_pointer<void>();
}

BOOST_AUTO_TEST_SUITE_END()
