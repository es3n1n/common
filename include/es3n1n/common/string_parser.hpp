#pragma once
#include "types.hpp"

#include <any>
#include <cassert>
#include <charconv>
#include <cstdint>
#include <format>
#include <stdexcept>
#include <string>

namespace string_parser {
    namespace detail {
        /// \brief Removes the "0x" prefix from hexadecimal strings
        /// \param str The string to process
        /// \param is_negative Whether the number is negative
        void strip_hex_prefix(std::string& str, bool is_negative) {
            /// We trust the is_negative, we also trust that the input is indeed in hexadecimal form.
            if (str.size() < 2) {
                return;
            }

            /// For negative hex values we can leave the - sign however we must remove the 0x
            if (is_negative) {
                assert(str.size() >= 3);
                str.erase(1, 2);
                return;
            }

            /// Remove 0x
            str.erase(0, 2);
        }

        /// \brief Parses a string to an integral type using std::from_chars
        /// \tparam Ty The integral type to parse to
        /// \param s The string to parse
        /// \param base The numeric base of the string (default: 10)
        /// \return The parsed integral value
        /// \throws std::invalid_argument if parsing fails due to invalid input
        /// \throws std::out_of_range if the parsed value is out of range for the type
        template <std::integral Ty>
        [[nodiscard]] Ty parse_from_chars(std::string s, int base = 10) {
            const auto is_negative = s.front() == '-';
            const auto is_hex = base == 16;

            if (is_hex) {
                strip_hex_prefix(s, is_negative);
            }

            Ty result;
            auto [ptr, ec] = std::from_chars(s.data(), s.data() + s.size(), result, base);
            if (ec == std::errc::invalid_argument) {
                throw std::invalid_argument("Failed to parse integer from string: invalid argument");
            } else if (ec == std::errc::result_out_of_range) {
                throw std::out_of_range("Failed to parse integer from string: out of range");
            }

            assert(ec == std::errc{});
            return result;
        }
    } // namespace detail

    /// \brief Parse int32 from string
    /// \param s String containing an int32 value
    /// \param base Numeric base (10 for decimal, 16 for hex, etc.)
    /// \return Parsed int32 value
    [[nodiscard]] inline std::int32_t parse_int32(const std::string_view s, const std::size_t base = 10) {
        return detail::parse_from_chars<std::int32_t>(std::string(s), static_cast<int>(base));
    }

    /// \brief Parse uint32 from string
    /// \param s String containing a uint32 value
    /// \param base Numeric base (10 for decimal, 16 for hex, etc.)
    /// \return Parsed uint32 value
    [[nodiscard]] inline std::uint32_t parse_uint32(const std::string_view s, const std::size_t base = 10) {
        return detail::parse_from_chars<std::uint32_t>(std::string(s), static_cast<int>(base));
    }

    /// \brief Parse int8 from string
    /// \param s String containing an int8 value
    /// \param base Numeric base (10 for decimal, 16 for hex, etc.)
    /// \return Parsed int8 value
    [[nodiscard]] inline std::int8_t parse_int8(const std::string_view s, const std::size_t base = 10) {
        return static_cast<std::int8_t>(parse_int32(s, base) & 0xFF);
    }

    /// \brief Parse uint8 from string
    /// \param s String containing a uint8 value
    /// \param base Numeric base (10 for decimal, 16 for hex, etc.)
    /// \return Parsed uint8 value
    [[nodiscard]] inline std::uint8_t parse_uint8(const std::string_view s, const std::size_t base = 10) {
        return parse_uint32(s, base) & 0xFF;
    }

    /// \brief Parse bool from string
    /// \param s String containing a boolean value
    /// \return Parsed boolean value
    [[nodiscard]] inline bool parse_bool(const std::string_view s) {
        return s == "true" || s == "1";
    }

    namespace detail {
        template <typename Ty, typename TyNoCV = std::remove_cv_t<Ty>>
        struct serializer_ctx {
            constexpr static bool is_int32 = std::is_same_v<TyNoCV, int> || std::is_same_v<TyNoCV, long>;
            constexpr static bool is_uint32 = std::is_same_v<TyNoCV, unsigned int> || std::is_same_v<TyNoCV, unsigned long>;
            constexpr static bool is_int8 = std::is_same_v<TyNoCV, char>;
            constexpr static bool is_uint8 = std::is_same_v<TyNoCV, unsigned char>;

            constexpr static bool is_number = is_int32 || is_uint32 || is_int8 || is_uint8;
            constexpr static bool is_bool = std::is_same_v<TyNoCV, bool>;
        };
    } // namespace detail

    /// \brief Parse a value from string using a template
    /// \tparam Ty Type to parse the string into
    /// \param s String to parse
    /// \return Parsed value of type Ty
    template <typename Ty, typename Ctx = detail::serializer_ctx<Ty>>
    [[nodiscard]] Ty parse(const std::string_view s) {
        if constexpr (Ctx::is_int32) {
            return parse_int32(s);
        } else if constexpr (Ctx::is_uint32) {
            return parse_uint32(s);
        } else if constexpr (Ctx::is_bool) {
            return parse_bool(s);
        } else if constexpr (Ctx::is_int8) {
            return parse_int8(s);
        } else if constexpr (Ctx::is_uint8) {
            return parse_uint8(s);
        } else {
            static_assert(traits::always_false_v<Ty>, "Unknown type to parse");
            return {};
        }
    }

    /// \brief Serialize a value to string
    /// \tparam Ty Type of the value to serialize
    /// \param value Value to serialize
    /// \return Serialized string representation of the value
    template <typename Ty, typename Ctx = detail::serializer_ctx<Ty>>
    [[nodiscard]] std::string serialize(const Ty value) {
        if constexpr (Ctx::is_number) {
            return std::to_string(value);
        } else if constexpr (Ctx::is_bool) {
            return value ? "true" : "false";
        } else {
            static_assert(traits::always_false_v<Ty>, "Unknown type to serialize");
            return {};
        }
    }

    /// \brief Parse a string and store the result in a std::any
    /// \param out Reference to the std::any where the result will be stored
    /// \param s String to parse
    /// \throws std::runtime_error if the type is unsupported
    inline void parse_to_any(std::any& out, const std::string_view s) {
        assert(out.has_value());

        const auto hash = out.type().hash_code();

        /// This is ugly, however, i can't calculate typeid(T).hash_code() in constexpr so
        /// no optimized switch cases :shrug: (could be implementation defined)
#define MAKE_CASE(type)                     \
    if (hash == typeid(type).hash_code()) { \
        out.emplace<type>(parse<type>(s));  \
        return;                             \
    }

        MAKE_CASE(int)
        MAKE_CASE(unsigned int)
        MAKE_CASE(long)
        MAKE_CASE(unsigned long)
        MAKE_CASE(bool)

#undef MAKE_CASE

        throw std::runtime_error(std::format("parse_to_any: Unable to parse '{}' -> unsupported type", s));
    }

    /// \brief Serialize a std::any value to string
    /// \param ref Reference to the std::any to serialize
    /// \return Serialized string representation of the value
    /// \throws std::runtime_error if the type is unsupported
    [[nodiscard]] inline std::string serialize_any(const std::any& ref) {
        assert(ref.has_value());
        const auto hash = ref.type().hash_code();

        /// This is ugly, however, i can't calculate typeid(T).hash_code() in constexpr so
        /// no optimized switch cases :shrug: (could be implementation defined)
#define MAKE_CASE(type)                                   \
    if (hash == typeid(type).hash_code()) {               \
        return serialize<type>(std::any_cast<type>(ref)); \
    }

        MAKE_CASE(int)
        MAKE_CASE(unsigned int)
        MAKE_CASE(long)
        MAKE_CASE(unsigned long)
        MAKE_CASE(bool)

#undef MAKE_CASE

        throw std::runtime_error("serialize_any: Unable to serialize -> unsupported type");
    }
} // namespace string_parser
