/**
 * @file
 *
 * Contains facilities used to represent metadata used by plugins and
 * components.
 */

module;

#include <boost/mp11/list.hpp>
#include <boost/url/parse.hpp>

#include <synthclone/config.h>

export module synthclone.core:metadata;

import std;

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

    constexpr char32_t line_feed = '\n';
    constexpr char32_t vertical_tab = '\v';
    constexpr char32_t form_feed = '\f';
    constexpr char32_t carriage_return = '\r';
    constexpr char32_t next_line = U'\U00000085';
    constexpr char32_t line_separator = U'\U00002028';
    constexpr char32_t paragraph_separator = U'\U00002029';

    std::string&&
    verify_metadata_element(std::string&& s)
    {
        if (s.empty()) [[unlikely]] {
            throw metadata_error("metadata element strings cannot be empty");
        }
        std::ranges::for_each(
            decode_utf8(std::as_bytes(std::span(s))),
            [](const unicode_codepoint c) {
                switch (c.value()) {
                case line_feed:
                case vertical_tab:
                case form_feed:
                case carriage_return:
                case next_line:
                case line_separator:
                case paragraph_separator:
                    throw metadata_error(
                        std::format(
                            "codepoint {0} must not be present in a metadata "
                            "element",
                            static_cast<std::uint_least32_t>(c)));

                [[likely]] default:
                    ;
                }
            });

        return std::move(s);
    }

    /**
     * A metadata string type that can hold any UTF-8 encoded data except line
     * terminators.
     *
     * Data in a `metadata_element` should be brief and succinct.
     */

    export
    class metadata_element final: public string_proxy<metadata_element> {

    public:

        /**
         * Constructs a `metadata_element` instance.
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
        metadata_element(Args&&... args):
            string_proxy<metadata_element>(
                verify_metadata_element(
                    std::string(std::forward<Args>(args)...)))
        {
            // empty
        }

    };

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
                    "boost::url::parse_absolute_uri({0:?}): failed to parse "
                    "metadata URL: {1}",
                    s, result.error().message()));
        }

        const auto& url_view = result.value();

        assume(
            url_view.has_scheme(),
            "boost::url::parse_absolute_uri({0:?}) returned url view with "
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
// synthclone::metadata_init_args
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    using metadata_element_sequence = std::vector<metadata_element>;

    /**
     * Used to initialize `metadata` instances using aggregate initialization.
     */

    export
    struct metadata_init_args final {

        /**
         * The plugin or component identifier.
         */

        metadata_element identifier;

        /**
         * The plugin or component version.
         */

        metadata_element version;

        /**
         * The optional title for the plugin or component.
         */

        std::optional<metadata_element> title;

        /**
         * The optional URL that links to more information on the plugin or
         * component.
         */

        std::optional<metadata_url> url;

        /**
         * The optional license descriptor for the plugin or component.
         */

        std::optional<metadata_element> license;

        /**
         * The tiered category for the plugin or component.
         */

        metadata_element_sequence category;

        /**
         * The creators of the plugin or component.
         */

        metadata_element_sequence creators;

        /**
         * The contributors to the plugin or component.
         */

        metadata_element_sequence contributors;

        /**
         * The description of the plugin or component.
         */

        metadata_text description;

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::metadata
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Contains metadata for a plugin or a component.
     */

    export
    class metadata final {

    public:

        /**
         * Constructs a `metadata` instance.
         *
         * @param args
         *   The data to use to populate the `metadata` instance.
         */

        constexpr
        metadata(metadata_init_args args)
        noexcept:
            license_(std::move(args.license)),
            title_(std::move(args.title)),
            url_(std::move(args.url)),
            category_(std::move(args.category)),
            contributors_(std::move(args.contributors)),
            creators_(std::move(args.creators)),
            description_(std::move(args.description)),
            identifier_(std::move(args.identifier)),
            version_(std::move(args.version))
        {
            // empty
        }

        /**
         * Gets the tiered category for the plugin or component.
         *
         * @return
         *   A range representing the tiered category.
         */

        constexpr
        auto
        category()
        const
        {
            return std::ranges::subrange(category_.cbegin(), category_.cend());
        }

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

    private:

        std::optional<metadata_element> license_;
        std::optional<metadata_element> title_;
        std::optional<metadata_url> url_;

        metadata_element_sequence category_;
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
     */

    export
    constexpr
    bool
    operator==(const metadata& lhs, const metadata& rhs)
    {
        return (lhs.identifier() == rhs.identifier()) &&
            (lhs.version() == rhs.version()) &&
            (lhs.title() == rhs.title()) &&
            (lhs.url() == rhs.url()) &&
            (lhs.license() == rhs.license()) &&
            std::ranges::equal(lhs.category(), rhs.category()) &&
            std::ranges::equal(lhs.creators(), rhs.creators()) &&
            std::ranges::equal(lhs.contributors(), rhs.contributors()) &&
            (lhs.description() == rhs.description());
    }

    /**
     * Gets a boolean indicating whether or not two `metadata` instances
     * contain different metadata.
     *
     * @param lhs
     *   The first instance.
     * @param rhs
     *   The second instance.
     *
     * @return
     *   The boolean indicator.
     */

    export
    constexpr
    bool
    operator!=(const metadata& lhs, const metadata& rhs)
    {
        return ! (lhs == rhs);
    }

}
