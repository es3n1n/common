#pragma once
#include "es3n1n/common/hashes/base.hpp"

namespace hashes {
    /// \brief FNV hash parameters
    /// \tparam Ty The size type for the hash
    template <detail::HashSize Ty>
    struct FnvParameters {
        /// \brief The FNV prime
        static constexpr Ty prime = sizeof(Ty) == 4 ? 0x01000193 : 0x00000100000001b3;

        /// \brief The FNV basis
        static constexpr Ty basis = sizeof(Ty) == 4 ? 0x811C9DC5 : 0xcbf29CE484222325;
    };

    /// \brief Fowler–Noll–Vo-1 hash function
    /// \tparam Ty The size type for the hash
    /// \see https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
    template <detail::HashSize Ty = detail::DefaultHashSize>
    class Fnv1 : public HashFunction<Fnv1<Ty>, Ty> {
        using Parameters = FnvParameters<Ty>;

    public:
        /// \brief Implementation of the FNV-1 hash function
        /// \tparam CharTy The character type
        /// \param value The span of characters to hash
        /// \return The computed hash
        template <detail::Hashable CharTy>
        [[nodiscard]] static constexpr Ty hash_impl(const std::span<CharTy> value) noexcept {
            Ty result = Parameters::basis;
            for (auto& c : value) {
                result *= Parameters::prime;
                result ^= static_cast<Ty>(c);
            }
            return result;
        }
    };

    /// \brief Fowler–Noll–Vo-1a hash function
    /// \tparam Ty The size type for the hash
    /// \see https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
    template <detail::HashSize Ty = detail::DefaultHashSize>
    class Fnv1a : public HashFunction<Fnv1a<Ty>, Ty> {
        using Parameters = FnvParameters<Ty>;

    public:
        /// \brief Implementation of the FNV-1a hash function
        /// \tparam CharTy The character type
        /// \param value The span of characters to hash
        /// \return The computed hash
        template <detail::Hashable CharTy>
        [[nodiscard]] static constexpr Ty hash_impl(const std::span<CharTy> value) noexcept {
            Ty result = Parameters::basis;
            for (auto& c : value) {
                result ^= static_cast<Ty>(c);
                result *= Parameters::prime;
            }
            return result;
        }
    };

    /// \brief Type aliases for FNV-1 and FNV-1a hash functions
    using Fnv1_32 = Fnv1<std::uint32_t>;
    using Fnv1_64 = Fnv1<std::uint64_t>;
    using Fnv1a_32 = Fnv1a<std::uint32_t>;
    using Fnv1a_64 = Fnv1a<std::uint64_t>;
} // namespace hashes

/// \brief User-defined literal for FNV-1 32-bit hash
/// \param value The string to hash
/// \param size The size of the string
/// \return The computed 32-bit FNV-1 hash
inline consteval std::uint32_t operator""_fnv1_32(const char* value, std::size_t size) noexcept {
    return hashes::Fnv1_32::hash(std::span(value, size));
}

/// \brief User-defined literal for FNV-1 64-bit hash
/// \param value The string to hash
/// \param size The size of the string
/// \return The computed 64-bit FNV-1 hash
inline consteval std::uint64_t operator""_fnv1_64(const char* value, std::size_t size) noexcept {
    return hashes::Fnv1_64::hash(std::span(value, size));
}

/// \brief User-defined literal for FNV-1a 32-bit hash
/// \param value The string to hash
/// \param size The size of the string
/// \return The computed 32-bit FNV-1a hash
inline consteval std::uint32_t operator""_fnv1a_32(const char* value, std::size_t size) noexcept {
    return hashes::Fnv1a_32::hash(std::span(value, size));
}

/// \brief User-defined literal for FNV-1a 64-bit hash
/// \param value The string to hash
/// \param size The size of the string
/// \return The computed 64-bit FNV-1a hash
inline consteval std::uint64_t operator""_fnv1a_64(const char* value, std::size_t size) noexcept {
    return hashes::Fnv1a_64::hash(std::span(value, size));
}
