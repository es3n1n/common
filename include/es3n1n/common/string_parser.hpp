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
        inline void strip_hex_prefix(std::string& str, bool is_negative) {
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

        template <std::integral Ty>
        [[nodiscard]] Ty parse_from_chars(std::string s, int base = 10) {
            const auto is_negative = s.front() == '-';

            if (const auto is_hex = base == 16 || (s.size() >= 2 && s[0] == '0' && (s[1] == 'x' || s[1] == 'X'))) {
                strip_hex_prefix(s, is_negative);
                base = 16;
            }

            Ty result;
            auto [ptr, ec] = std::from_chars(s.data(), s.data() + s.size(), result, base);
            if (ec == std::errc::invalid_argument) {
                throw std::invalid_argument("Failed to parse integer from string: invalid argument");
            }
            if (ec == std::errc::result_out_of_range) {
                throw std::out_of_range("Failed to parse integer from string: out of range");
            }

            assert(ec == std::errc{});
            return result;
        }
    } // namespace detail

    [[nodiscard]] inline std::uint64_t parse_int64(const std::string_view s, const std::size_t base = 10) {
        return detail::parse_from_chars<std::int64_t>(std::string(s), static_cast<int>(base));
    }

    [[nodiscard]] inline std::uint64_t parse_uint64(const std::string_view s, const std::size_t base = 10) {
        return detail::parse_from_chars<std::uint64_t>(std::string(s), static_cast<int>(base));
    }

    [[nodiscard]] inline std::int32_t parse_int32(const std::string_view s, const std::size_t base = 10) {
        return detail::parse_from_chars<std::int32_t>(std::string(s), static_cast<int>(base));
    }

    [[nodiscard]] inline std::uint32_t parse_uint32(const std::string_view s, const std::size_t base = 10) {
        return detail::parse_from_chars<std::uint32_t>(std::string(s), static_cast<int>(base));
    }

    [[nodiscard]] inline std::int8_t parse_int8(const std::string_view s, const std::size_t base = 10) {
        return static_cast<std::int8_t>(parse_int32(s, base) & 0xFF);
    }

    [[nodiscard]] inline std::uint8_t parse_uint8(const std::string_view s, const std::size_t base = 10) {
        return parse_uint32(s, base) & 0xFF;
    }

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
