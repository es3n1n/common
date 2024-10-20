#pragma once
#include "es3n1n/common/hashes/base.hpp"
#include <array>
#include <climits>

namespace hashes {
    /// \brief Fowler–Noll–Vo-1 hash function
    /// \tparam Ty The size type for the hash
    /// \see https://web.mit.edu/freebsd/head/sys/libkern/crc32.c
    template <detail::HashSize Ty>
        requires(std::is_same_v<Ty, std::uint32_t>) // we might add more hash sizes in the future though
    class CrcB : public HashFunction<CrcB<Ty>, Ty> {
        /// \brief CRC-32 parameters
        static constexpr std::uint32_t kCrc32Polynomial = 0xEDB88320;
        static constexpr auto kCrc32Table = []() -> std::array<std::uint32_t, 0x100> {
            std::array<std::uint32_t, 0x100> result = {};
            for (std::size_t i = 0; i < 0x100; ++i) {
                auto crc = static_cast<std::uint32_t>(i);
                for (std::size_t bit = 0; bit < CHAR_BIT; bit++) {
                    crc = (crc >> 1) ^ ((crc & 1) * kCrc32Polynomial);
                }
                // \note: @annihilatorq: .at() is used to avoid weird warning C28020, when static
                // analyzer thinks the index may go out of bounds when using [], despite clear limits
                result.at( i ) = crc;
            }
            return result;
        }();

    public:
        /// \brief Implementation of the FNV-1 hash function
        /// \tparam CharTy The character type
        /// \param value The span of characters to hash
        /// \return The computed hash
        template <detail::Hashable CharTy>
        [[nodiscard]] static constexpr Ty hash_impl(const std::span<CharTy> value) noexcept {
            Ty result = ~Ty{0};
            for (auto& c : value) {
                std::size_t index = (result ^ static_cast<Ty>(c)) & 0xFF;
                result = kCrc32Table[index] ^ (result >> 8);
            }
            return ~result;
        }
    };

    /// \brief 32-bit CRCb hash
    using Crcb_32 = CrcB<std::uint32_t>;
} // namespace hashes

/// \brief User-defined literal for CRCb 32-bit hash
/// \param value The string to hash
/// \param size The size of the string
/// \return The computed 32-bit CRC hash
inline consteval std::uint32_t operator""_crcb_32(const char* value, std::size_t size) noexcept {
    return hashes::Crcb_32::hash(std::span(value, size));
}
