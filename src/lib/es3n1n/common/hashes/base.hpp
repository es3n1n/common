#pragma once
#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <string_view>

#include "es3n1n/common/traits.hpp"

namespace hashes {
    namespace detail {
        /// \brief Concept for hash size types
        template <typename Ty> concept HashSize = traits::is_any_of_v<Ty, std::size_t, std::uint32_t, std::uint64_t>;

        /// \brief Concept for types that can be hashed
        template <typename Ty> concept Hashable = traits::is_any_of_v<std::remove_cv_t<Ty>, std::uint8_t, char, wchar_t>;

        /// \brief Default hash size
        using DefaultHashSize = std::uint32_t;
    } // namespace detail

    /// \brief Base class for hash functions using CRTP
    /// \tparam Derived The derived hash function class
    /// \tparam Ty The size type for the hash
    template <typename Derived, detail::HashSize Ty = detail::DefaultHashSize>
    class HashFunction {
    public:
        /// \todo @es3n1n: Add requires clause to check for Derived::hash_impl
        class Value {
        private:
            Ty value_;

        public:
            /// \brief Construct from a pre-calculated hash value
            /* implicit */ consteval Value(Ty hash) noexcept: value_(hash) { }

            /// \brief Construct from a string (consteval)
            template <detail::Hashable CharTy>
            /* implicit */ consteval Value(const CharTy* str) noexcept: value_(Derived::hash(str)) { }

            /// \brief Implicit conversion to the hash type
            /* implicit */ constexpr operator Ty() const noexcept {
                return value_;
            }

            /// \brief Equality comparison
            constexpr bool operator==(const Value& other) const noexcept = default;
            constexpr bool operator==(const Ty other) const noexcept {
                return value_ == other;
            }

            /// \brief Get the underlying value
            constexpr Ty get() const noexcept {
                return value_;
            }
        };

        /// \brief Compute the hash of a span of characters
        /// \tparam CharTy The character type
        /// \param value The span of characters to hash
        /// \return The computed hash
        template <detail::Hashable CharTy>
        [[nodiscard]] static constexpr Ty hash(const std::span<CharTy> value) noexcept {
            return Derived::hash_impl(value);
        }

        /// \brief Function call operator for a span of characters
        /// \tparam CharTy The character type
        /// \param value The span of characters to hash
        /// \return The computed hash
        template <detail::Hashable CharTy>
        [[nodiscard]] constexpr Ty operator()(const std::span<CharTy> value) const noexcept {
            return hash(value);
        }

        /// \brief Compute the hash of a null-terminated string
        /// \tparam CharTy The character type
        /// \param value The string to hash
        /// \param size Optional size of the string
        /// \return The computed hash
        template <detail::Hashable CharTy>
        [[nodiscard]] static constexpr Ty hash(const CharTy* value, std::optional<std::size_t> size = std::nullopt) noexcept {
            if (!size.has_value()) {
                size = std::char_traits<CharTy>::length(value);
            }
            return hash(std::span(value, *size));
        }

        /// \brief Function call operator for a null-terminated string
        /// \tparam CharTy The character type
        /// \param value The string to hash
        /// \param size Optional size of the string
        /// \return The computed hash
        template <detail::Hashable CharTy>
        [[nodiscard]] constexpr Ty operator()(const CharTy* value, std::optional<std::size_t> size = std::nullopt) const noexcept {
            return hash(value, size);
        }

        /// \brief Compute the hash of a string
        /// \tparam CharTy The character type
        /// \param value The string to hash
        /// \return The computed hash
        template <detail::Hashable CharTy>
        [[nodiscard]] static constexpr Ty hash(const std::basic_string<CharTy>& value) noexcept {
            return hash(std::span(value.data(), value.size()));
        }

        /// \brief Function call operator for string
        /// \tparam CharTy The character type
        /// \param value The string to hash
        /// \return The computed hash
        template <detail::Hashable CharTy>
        [[nodiscard]] constexpr Ty operator()(const std::basic_string<CharTy>& value) const noexcept {
            return hash(value);
        }

        /// \brief Compute the hash of a string
        /// \tparam CharTy The character type
        /// \param value The string to hash
        /// \return The computed hash
        template <detail::Hashable CharTy>
        [[nodiscard]] static constexpr Ty hash(const std::basic_string_view<CharTy>& value) noexcept {
            return hash(std::span(value.data(), value.size()));
        }

        /// \brief Function call operator for string_view
        /// \tparam CharTy The character type
        /// \param value The string_view to hash
        /// \return The computed hash
        template <detail::Hashable CharTy>
        [[nodiscard]] constexpr Ty operator()(const std::basic_string_view<CharTy>& value) const noexcept {
            return hash(value);
        }
    };
} // namespace hashes
