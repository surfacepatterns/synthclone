/**
 * @file
 *
 * Contains facilities used to represent metadata used by plugins and
 * components.
 */

module;

#include <synthclone/config.h>

export module synthclone.core:metadata_core;

import std;

import synthclone.external.boost.mp11;
import synthclone.external.boost.urls;
import synthclone.util;

///////////////////////////////////////////////////////////////////////////////
// synthclone::metadata_error
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Exception class representing an issue parsing metadata.
     */

    export
    class metadata_error: public std::runtime_error {

    public:

        using std::runtime_error::runtime_error;

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::metadata_text
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * A metadata string type that can hold any UTF-8 encoded data.
     */

    export
    using metadata_text = utf8_string;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::metadata_element
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * A metadata string type that can hold any UTF-8 encoded data except line
     * terminators.
     *
     * Data in a `metadata_element` should be brief and succinct.
     */

    export
    using metadata_element = utf8_line;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::metadata_url
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    std::string&&
    verify_metadata_url(std::string&& s)
    {
        auto result = boost::urls::parse_absolute_uri(s);
        if (! result) [[unlikely]] {
            throw metadata_error(
                std::format(
                    "boost::urls::parse_absolute_uri({0:?}): failed to parse "
                    "metadata URL: {1}",
                    s, result.error().message()));
        }

        const auto& url_view = result.value();

        assume(
            url_view.has_scheme(),
            "boost::urls::parse_absolute_uri({0:?}) returned url view with "
            "empty scheme",
            s);

        if (! url_view.has_authority()) [[unlikely]] {
            throw metadata_error(
                std::format(
                    "{0:?}: metadata URL does not have an authority", s));
        }

        return std::move(s);
    }

    /**
     * A metadata string type that holds an absolute URL.
     */

    export
    class metadata_url final: public string_proxy<metadata_url> {

    public:

        /**
         * Constructs a `metadata_url` instance.
         *
         * @param args
         *   The arguments to pass to the `std::string` constructor.
         */

        template<class... Args>
        requires (std::constructible_from<std::string, Args...>)
        explicit (
            (sizeof...(Args) != 1) ||
            (
                ! implicitly_convertible_to<
                    boost::mp11::mp_front<boost::mp11::mp_list<Args...>>,
                    std::string
                >
            )
        )
        metadata_url(Args&&... args):
            string_proxy<metadata_url>(
                verify_metadata_url(std::string(std::forward<Args>(args)...)))
        {
            // empty
        }

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::metadata_element_sequence
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * The `metadata` element sequence type used by other `metadata` types.
     */

    export
    using metadata_element_sequence = std::vector<metadata_element>;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::metadata
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Contains base metadata for a plugin or a component.
     */

    export
    class metadata {

    public:

        /**
         * Gets the contributors to the plugin or component.
         *
         * @return
         *   A range containing the contributors.
         */

        constexpr
        auto
        contributors()
        const
        {
            return std::ranges::subrange(
                contributors_.cbegin(), contributors_.cend());
        }

        /**
         * Gets the creators of the plugin or component.
         *
         * @return
         *   A range containing the creators.
         */

        constexpr
        auto
        creators()
        const
        {
            return std::ranges::subrange(creators_.cbegin(), creators_.cend());
        }

        /**
         * Gets the description of the plugin or component.
         */

        constexpr
        const metadata_text&
        description()
        const noexcept
        {
            return description_;
        }

        /**
         * Gets the plugin or component identifier.
         *
         * @return
         *   The identifier.
         */

        constexpr
        const metadata_element&
        identifier()
        const noexcept
        {
            return identifier_;
        }

        /**
         * Gets the optional license descriptor for the plugin or component.
         *
         * @return
         *   The optional license descriptor.
         */

        constexpr
        const std::optional<metadata_element>&
        license()
        const noexcept
        {
            return license_;
        }

        /**
         * Gets the optional title for the plugin or component.
         *
         * @return
         *   The optional title.
         */

        constexpr
        const std::optional<metadata_element>&
        title()
        const noexcept
        {
            return title_;
        }

        /**
         * Gets the optional URL that links to more information on the plugin
         * or component.
         *
         * @return
         *   The optional URL.
         */

        constexpr
        const std::optional<metadata_url>&
        url()
        const noexcept
        {
            return url_;
        }

        /**
         * Gets the plugin or component version.
         *
         * @return
         *   The version.
         */

        constexpr
        const metadata_element&
        version()
        const noexcept
        {
            return version_;
        }

    protected:

        metadata(metadata&&) = default;

        metadata(const metadata&) = default;

        constexpr
        metadata(
            metadata_element&& identifier,
            metadata_element&& version,
            std::optional<metadata_element>&& title,
            std::optional<metadata_url>&& url,
            std::optional<metadata_element>&& license,
            metadata_element_sequence&& creators,
            metadata_element_sequence&& contributors,
            metadata_text&& description
        ) noexcept:
            license_(std::move(license)),
            title_(std::move(title)),
            url_(std::move(url)),
            contributors_(std::move(contributors)),
            creators_(std::move(creators)),
            description_(std::move(description)),
            identifier_(std::move(identifier)),
            version_(std::move(version))
        {
            // empty
        }

        metadata&
        operator=(metadata&&) = default;

    private:

        metadata&
        operator=(const metadata&) = delete;

        std::optional<metadata_element> license_;
        std::optional<metadata_element> title_;
        std::optional<metadata_url> url_;

        metadata_element_sequence creators_;
        metadata_element_sequence contributors_;

        metadata_text description_;
        metadata_element identifier_;
        metadata_element version_;

    };

    /**
     * Gets a boolean indicating whether or not two `metadata` instances
     * contain exactly the same metadata.
     *
     * @param lhs
     *   The first instance.
     * @param rhs
     *   The second instance.
     *
     * @return
     *   The boolean indicator.
     *
     * @note
     *   If we spell this function `operator==`, there's a bug in Clang wherein
     *   a coroutine handle compare in `std::ranges::elements_of` will be
     *   flagged as ambiguous, with *this* function being one of the candidates
     *   for comparison, despite the fact that `synthclone::metadata` is
     *   obviously not a coroutine handle type.  We spell this function `equal`
     *   to get around this bug.  The only reason this is acceptable is that
     *   this function doesn't get exported out of `synthclone.core`.
     */

    export
    constexpr
    bool
    equal(const metadata& lhs, const metadata& rhs)
    {
        return (lhs.identifier() == rhs.identifier()) &&
            (lhs.version() == rhs.version()) &&
            (lhs.title() == rhs.title()) &&
            (lhs.url() == rhs.url()) &&
            (lhs.license() == rhs.license()) &&
            std::ranges::equal(lhs.creators(), rhs.creators()) &&
            std::ranges::equal(lhs.contributors(), rhs.contributors()) &&
            (lhs.description() == rhs.description());
    }

}
