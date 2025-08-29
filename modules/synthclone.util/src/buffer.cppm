/**
 * @file
 *
 * Contains buffer data structures.
 */

module;

#include <synthclone/config.h>

export module synthclone.util:buffer;

import std;

import :debug;
import :utility;

///////////////////////////////////////////////////////////////////////////////
// synthclone::bufferable
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Indicates whether a type can be buffered, meaning the type:
     *   * is default initializable
     *   * is trivially copyable
     *   * is trivially destructible
     *
     * @tparam T
     *   The type to check.
     */

    export
    template<class T>
    concept bufferable =
        std::default_initializable<T> && std::is_trivially_copyable_v<T> &&
        std::is_trivially_destructible_v<T>;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::dynamic_buffer
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    // This is a slightly modified version of the allocator found in this
    // StackOverflow answer:
    //
    //     https://stackoverflow.com/questions/21028299/ \
    //     is-this-behavior-of-vectorresizesize-type-n-under-c11-and-boost- \
    //     container/21028912#21028912

    template<class T>
    class dynamic_buffer_allocator: public std::allocator<T> {

    public:

        using std::allocator<T>::allocator;

        template<class U>
        void
        construct(U* ptr)
        noexcept(std::is_nothrow_default_constructible_v<U>)
        {
            ::new(static_cast<void*>(ptr)) U;
        }

        template<class U, class...Args>
        void
        construct(U* ptr, Args&&... args)
        {
            std::allocator_traits<std::allocator<T>>::construct(
                static_cast<std::allocator<T>&>(*this), ptr,
                std::forward<Args>(args)...);
        }

    };

    template<bufferable T>
    using dynamic_buffer_impl = std::vector<T, dynamic_buffer_allocator<T>>;

    /**
     * Variable sized buffer.
     *
     * @tparam T
     *   The buffer element type.
     *
     * @note
     *   The class is implemented as a thin wrapper around a vector with a
     *   custom allocator that avoids value initialization of elements.
     */

    export
    template<bufferable T>
    class dynamic_buffer final: private noncopyable {

    public:

        /**
         * Iterator used to iterate over elements in the buffer.
         */

        using const_iterator = dynamic_buffer_impl<T>::const_iterator;

        /**
         * Element pointer type.
         */

        using const_pointer = dynamic_buffer_impl<T>::const_pointer;

        /**
         * Element reference type.
         */

        using const_reference = dynamic_buffer_impl<T>::const_reference;

        /**
         * Iterator used to iterate over elements in the buffer in reverse
         * order.
         */

        using const_reverse_iterator =
            dynamic_buffer_impl<T>::const_reverse_iterator;

        /**
         * Type used to measure the difference between two iterators.
         */

        using difference_type = dynamic_buffer_impl<T>::difference_type;

        /**
         * Iterator used to iterate over elements in the buffer.
         */

        using iterator = dynamic_buffer_impl<T>::iterator;

        /**
         * Element pointer type.
         */

        using pointer = dynamic_buffer_impl<T>::pointer;

        /**
         * Element reference type.
         */

        using reference = dynamic_buffer_impl<T>::reference;

        /**
         * Iterator used to iterate over elements in the buffer in reverse
         * order.
         */

        using reverse_iterator = dynamic_buffer_impl<T>::reverse_iterator;

        /**
         * Type used to represent the length of the buffer.
         */

        using size_type = dynamic_buffer_impl<T>::size_type;

        /**
         * The element type.
         */

        using value_type = dynamic_buffer_impl<T>::value_type;

        /**
         * Default constructor.
         */

        dynamic_buffer() = default;

        /**
         * Constructs a `dynamic_buffer` instance with the given initial size.
         *
         * @param size
         *   The size.
         */

        explicit
        dynamic_buffer(size_type size):
            impl_(size)
        {
            // empty
        }

        /**
         * Gets the element at the specified position.
         *
         * @param pos
         *   The position.
         *
         * @return
         *   The element.
         */

        constexpr
        const_reference
        operator[](size_type n)
        const
        {
            assume(
                n < size(), "index ({0}) is not less than buffer size ({1})",
                n, size());

            return impl_[n];
        }

        /**
         * Gets the element at the specified position.
         *
         * @param pos
         *   The position.
         *
         * @return
         *   The element.
         */

        constexpr
        reference
        operator[](size_type n)
        {
            assume(
                n < size(), "index ({0}) is not less than buffer size ({1})",
                n, size());

            return impl_[n];
        }

        /**
         * Gets the element at the specified position.
         *
         * @param pos
         *   The position.
         *
         * @return
         *   The element.
         */

        constexpr
        const_reference
        at(size_type n)
        const
        {
            verify(
                n < size(), "index ({0}) is not less than buffer size ({1})",
                n, size());

            return impl_[n];
        }

        /**
         * Gets the element at the specified position.
         *
         * @param pos
         *   The position.
         *
         * @return
         *   The element.
         */

        constexpr
        reference
        at(size_type n)
        {
            verify(
                n < size(), "index ({0}) is not less than buffer size ({1})",
                n, size());

            return impl_[n];
        }

        /**
         * Gets an iterator pointing at the beginning of the buffer.
         *
         * @return
         *   The iterator.
         */

        constexpr
        const_iterator
        begin()
        const noexcept
        {
            return cbegin();
        }

        /**
         * Gets an iterator pointing at the beginning of the buffer.
         *
         * @return
         *   The iterator.
         */

        constexpr
        iterator
        begin()
        noexcept
        {
            return impl_.begin();
        }

        /**
         * Gets the number of elements that the buffer has allocated space for,
         * which *may* be different than the buffer size.
         *
         * @return
         *   The capacity.
         */

        constexpr
        size_type
        capacity()
        const noexcept
        {
            return impl_.capacity();
        }

        /**
         * Gets an iterator pointing at the beginning of the buffer.
         *
         * @return
         *   The iterator.
         */

        constexpr
        const_iterator
        cbegin()
        const noexcept
        {
            return impl_.cbegin();
        }

        /**
         * Gets an iterator pointing at the end of the buffer.
         *
         * @return
         *   The iterator.
         */

        constexpr
        const_iterator
        cend()
        const noexcept
        {
            return impl_.cend();
        }

        /**
         * Gets a reverse iterator pointing at the end of the buffer.
         *
         * @return
         *   The iterator.
         */

        constexpr
        const_reverse_iterator
        crbegin()
        const noexcept
        {
            return impl_.crbegin();
        }

        /**
         * Gets a reverse iterator pointing at the beginning of the buffer.
         *
         * @return
         *   The iterator.
         */

        constexpr
        const_reverse_iterator
        crend()
        const noexcept
        {
            return impl_.crend();
        }

        /**
         * Gets a pointer to the raw buffer data.
         *
         * @return
         *   The pointer.
         */

        constexpr
        const_pointer
        data()
        const noexcept
        {
            return impl_.data();
        }

        /**
         * Gets a pointer to the raw buffer data.
         *
         * @return
         *   The pointer.
         */

        constexpr
        pointer
        data()
        noexcept
        {
            return impl_.data();
        }

        /**
         * Gets a boolean indicating whether the buffer is empty.
         *
         * @return
         *   The boolean indicator.
         */

        [[nodiscard]]
        constexpr
        bool
        empty()
        const noexcept
        {
            return impl_.empty();
        }

        /**
         * Gets an iterator pointing at the end of the buffer.
         *
         * @return
         *   The iterator.
         */

        constexpr
        const_iterator
        end()
        const noexcept
        {
            return cend();
        }

        /**
         * Gets an iterator pointing at the end of the buffer.
         *
         * @return
         *   The iterator.
         */

        constexpr
        iterator
        end()
        noexcept
        {
            return impl_.end();
        }

        /**
         * Gets the maximum possible buffer size.
         *
         * @return
         *   The maximum possible buffer size.
         */

        constexpr
        size_type
        max_size()
        const noexcept
        {
            return impl_.max_size();
        }

        /**
         * Gets a reverse iterator pointing at the end of the buffer.
         *
         * @return
         *   The iterator.
         */

        constexpr
        reverse_iterator
        rbegin()
        noexcept
        {
            return impl_.rbegin();
        }

        /**
         * Gets a reverse iterator pointing at the beginning of the buffer.
         *
         * @return
         *   The iterator.
         */

        constexpr
        reverse_iterator
        rend()
        noexcept
        {
            return impl_.rend();
        }

        /**
         * Increases the capacity of the buffer to a value that's greater than
         * or equal to the given capacity.  Note that this does *not* change
         * the buffer's size - use `resize` to change the buffer's size.
         *
         * @param n
         *   The requested capacity.
         */

        void
        reserve(size_type n)
        {
            impl_.reserve(n);
        }

        /**
         * Resizes the buffer.
         *
         * @param n
         *   The updated size.
         */

        void
        resize(size_type n)
        {
            impl_.resize(n);
        }

        /**
         * Requests the removal of unused capacity.
         */

        constexpr
        void
        shrink_to_fit()
        {
            impl_.shrink_to_fit();
        }

        /**
         * Gets the buffer size.
         *
         * @return
         *   The size.
         */

        constexpr
        size_type
        size()
        const noexcept
        {
            return impl_.size();
        }

        /**
         * Swaps the contents of this buffer and the given buffer.
         *
         * @param other
         *   The buffer to swap with.
         */

        constexpr
        void
        swap(dynamic_buffer& other)
        noexcept
        {
            impl_.swap(other.impl_);
        }

    private:

        dynamic_buffer_impl<T> impl_;

    };

    /**
     * Swaps the contents of the two given buffers.
     *
     * @param lhs
     *   The first buffer.
     * @param rhs
     *   The second buffer.
     */

    export
    template<bufferable T>
    constexpr
    void
    swap(dynamic_buffer<T>& lhs, dynamic_buffer<T>& rhs)
    noexcept
    {
        lhs.swap(rhs);
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::fixed_buffer
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    template<bufferable T>
    using fixed_buffer_impl = std::unique_ptr<T[]>;

    /**
     * Fixed sized buffer.
     *
     * @tparam T
     *   The buffer element type.
     * @tparam N
     *   The fixed buffer size.
     */

    export
    template<bufferable T, std::size_t N>
    class fixed_buffer final: private noncopyable {

        static_assert(N != 0);

    public:

        /**
         * The element type.
         */

        using value_type = fixed_buffer_impl<T>::element_type;

        /**
         * Element pointer type.
         */

        using const_pointer = const value_type*;

        /**
         * Element pointer type.
         */

        using pointer = value_type*;

        /**
         * Iterator used to iterate over elements in the buffer.
         */

        using iterator = pointer;

        /**
         * Iterator used to iterate over elements in the buffer.
         */

        using const_iterator = std::const_iterator<iterator>;

        /**
         * Element reference type.
         */

        using const_reference = const value_type&;

        /**
         * Iterator used to iterate over elements in the buffer in reverse
         * order.
         */

        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        /**
         * Type used to measure the difference between two iterators.
         */

        using difference_type =
            std::pointer_traits<fixed_buffer_impl<T>>::difference_type;

        /**
         * Element reference type.
         */

        using reference = value_type&;

        /**
         * Iterator used to iterate over elements in the buffer in reverse
         * order.
         */

        using reverse_iterator = std::reverse_iterator<iterator>;

        /**
         * Type used to represent the length of the buffer.
         */

        using size_type = std::make_unsigned_t<difference_type>;

        /**
         * Default constructor.
         */

        constexpr
        fixed_buffer():
            impl_(std::make_unique_for_overwrite<value_type[]>(N))
        {
            // empty
        }

        /**
         * Gets the element at the specified position.
         *
         * @param pos
         *   The position.
         *
         * @return
         *   The element.
         */

        constexpr
        const_reference
        operator[](size_type n)
        const
        {
            assume(
                n < N, "index ({0}) is not less than buffer size ({1})", n, N);
            return impl_.get()[n];
        }

        /**
         * Gets the element at the specified position.
         *
         * @param pos
         *   The position.
         *
         * @return
         *   The element.
         */

        constexpr
        reference
        operator[](size_type n)
        {
            assume(
                n < N, "index ({0}) is not less than buffer size ({1})", n, N);
            return impl_.get()[n];
        }

        /**
         * Gets the element at the specified position.
         *
         * @param pos
         *   The position.
         *
         * @return
         *   The element.
         */

        constexpr
        const_reference
        at(size_type n)
        const
        {
            verify(
                n < N, "index ({0}) is not less than buffer size ({1})", n, N);
            return impl_.get()[n];
        }

        /**
         * Gets the element at the specified position.
         *
         * @param pos
         *   The position.
         *
         * @return
         *   The element.
         */

        constexpr
        reference
        at(size_type n)
        {
            verify(
                n < N, "index ({0}) is not less than buffer size ({1})", n, N);
            return impl_.get()[n];
        }

        /**
         * Gets an iterator pointing at the beginning of the buffer.
         *
         * @return
         *   The iterator.
         */

        constexpr
        const_iterator
        begin()
        const noexcept
        {
            return cbegin();
        }

        /**
         * Gets an iterator pointing at the beginning of the buffer.
         *
         * @return
         *   The iterator.
         */

        constexpr
        iterator
        begin()
        noexcept
        {
            return impl_.get();
        }

        /**
         * Gets an iterator pointing at the beginning of the buffer.
         *
         * @return
         *   The iterator.
         */

        constexpr
        const_iterator
        cbegin()
        const noexcept
        {
            return impl_.get();
        }

        /**
         * Gets an iterator pointing at the end of the buffer.
         *
         * @return
         *   The iterator.
         */

        constexpr
        const_iterator
        cend()
        const noexcept
        {
            return impl_.get() + N;
        }

        /**
         * Gets a reverse iterator pointing at the end of the buffer.
         *
         * @return
         *   The iterator.
         */

        constexpr
        const_reverse_iterator
        crbegin()
        const noexcept
        {
            return const_reverse_iterator(cend());
        }

        /**
         * Gets a reverse iterator pointing at the beginning of the buffer.
         *
         * @return
         *   The iterator.
         */

        constexpr
        const_reverse_iterator
        crend()
        const noexcept
        {
            return const_reverse_iterator(cbegin());
        }

        /**
         * Gets a pointer to the raw buffer data.
         *
         * @return
         *   The pointer.
         */

        constexpr
        const_pointer
        data()
        const noexcept
        {
            return impl_.get();
        }

        /**
         * Gets a pointer to the raw buffer data.
         *
         * @return
         *   The pointer.
         */

        constexpr
        pointer
        data()
        noexcept
        {
            return impl_.get();
        }

        /**
         * Gets a boolean indicating whether the buffer is empty.
         *
         * @return
         *   The boolean indicator.
         */

        [[nodiscard]]
        constexpr
        bool
        empty()
        const noexcept
        {
            return false;
        }

        /**
         * Gets an iterator pointing at the end of the buffer.
         *
         * @return
         *   The iterator.
         */

        constexpr
        const_iterator
        end()
        const noexcept
        {
            return cend();
        }

        /**
         * Gets an iterator pointing at the end of the buffer.
         *
         * @return
         *   The iterator.
         */

        constexpr
        iterator
        end()
        noexcept
        {
            return impl_.get() + N;
        }

        /**
         * Gets the maximum possible buffer size.
         *
         * @return
         *   The maximum possible buffer size.
         */

        constexpr
        size_type
        max_size()
        const noexcept
        {
            return N;
        }

        /**
         * Gets a reverse iterator pointing at the end of the buffer.
         *
         * @return
         *   The iterator.
         */

        constexpr
        reverse_iterator
        rbegin()
        noexcept
        {
            return reverse_iterator(end());
        }

        /**
         * Gets a reverse iterator pointing at the beginning of the buffer.
         *
         * @return
         *   The iterator.
         */

        constexpr
        reverse_iterator
        rend()
        noexcept
        {
            return reverse_iterator(begin());
        }

        /**
         * Gets the buffer size.
         *
         * @return
         *   The size.
         */

        constexpr
        size_type
        size()
        const noexcept
        {
            return N;
        }

        /**
         * Swaps the contents of this buffer and the given buffer.
         *
         * @param other
         *   The buffer to swap with.
         */

        constexpr
        void
        swap(fixed_buffer& other)
        noexcept
        {
            impl_.swap(other.impl_);
        }

    private:

        fixed_buffer_impl<value_type> impl_;

    };

    /**
     * Swaps the contents of the two given buffers.
     *
     * @param lhs
     *   The first buffer.
     * @param rhs
     *   The second buffer.
     */

    export
    template<bufferable T, std::size_t N>
    constexpr
    void
    swap(fixed_buffer<T, N>& lhs, fixed_buffer<T, N>& rhs)
    noexcept
    {
        lhs.swap(rhs);
    }

}
