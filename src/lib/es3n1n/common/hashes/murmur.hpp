#pragma once
#include "es3n1n/common/hashes/base.hpp"
#include "es3n1n/common/numeric.hpp"
#include <array>

namespace hashes {
    namespace detail {
        /// \brief Murmur3 hash function parameters
        template <detail::HashSize Ty, std::endian Endian = std::endian::little>
        struct MurmurParameters {
            /// \brief Input data chunks will be converted to the endianness specified by this parameter
            static constexpr std::endian endian = Endian;

            /// \brief The seed constants
            static constexpr Ty c1 = sizeof(Ty) == 4 ? 0xcc9e2d51 : 0x87c37b91114253d5;
            static constexpr Ty c2 = sizeof(Ty) == 4 ? 0x1b873593 : 0x4cf5ad432745937f;

            /// \brief The shift and rotation constants
            static constexpr Ty r1 = sizeof(Ty) == 4 ? 15 : 31;
            static constexpr Ty r2 = sizeof(Ty) == 4 ? 13 : 27;
            static constexpr Ty m = 5;
            static constexpr Ty n = 0xe6546b64;

            /// \brief The finalization constants
            static constexpr Ty fmix_c1 = sizeof(Ty) == 4 ? 0x85ebca6b : 0xff51afd7ed558ccd;
            static constexpr Ty fmix_c2 = sizeof(Ty) == 4 ? 0xc2b2ae35 : 0xc4ceb9fe1a85ec53;
            static constexpr Ty fmix_shift_1 = sizeof(Ty) == 4 ? 16 : 33;
            static constexpr Ty fmix_shift_2 = sizeof(Ty) == 4 ? 13 : 33;
            static constexpr Ty fmix_shift_3 = sizeof(Ty) == 4 ? 16 : 33;
        };
    } // namespace detail

    /// \brief Murmur3 hash function
    /// \tparam Ty The size type for the hash
    /// \see https://en.wikipedia.org/wiki/MurmurHash
    template <detail::HashSize Ty, Ty Seed = 0, typename Parameters = detail::MurmurParameters<Ty>>
        requires(std::is_same_v<Ty, std::uint32_t>) /// \todo @es3n1n: Add support for 128-bit hashes once we have a 128-bit integer type
    class Murmur3 : public HashFunction<Murmur3<Ty>, Ty> {
    private:
        template <detail::Hashable CharTy>
        [[nodiscard]] static constexpr Ty read_le(const std::span<CharTy>& value, const std::size_t offset) noexcept {
            Ty result = 0;
            for (std::size_t i = 0; i < sizeof(Ty); ++i) {
                result |= static_cast<Ty>(static_cast<unsigned char>(value[offset + i])) << (i * 8);
            }
            return numeric::to_endian(result, Parameters::endian);
        }

    public:
        /// \brief Implementation of the Murmur3 hash function
        /// \tparam CharTy The character type
        /// \param value The span of characters to hash
        /// \return The computed hash
        template <detail::Hashable CharTy>
        [[nodiscard]] static constexpr Ty hash_impl(const std::span<CharTy> value) noexcept {
            Ty h = Seed;

            const std::size_t len = value.size() * sizeof(CharTy);
            const std::size_t num_blocks = len / sizeof(Ty);

            for (std::size_t i = 0; i < num_blocks; ++i) {
                auto k = read_le(value, i * sizeof(Ty));

                k *= Parameters::c1;
                k = std::rotl(k, Parameters::r1);
                k *= Parameters::c2;

                h ^= k;
                h = std::rotl(h, Parameters::r2);
                h = h * Parameters::m + Parameters::n;
            }

            const auto tail = value.subspan(value.size() - value.size() % sizeof(Ty));
            Ty k = 0;

            switch (value.size() & (sizeof(Ty) - 1)) {
            case 4:
                k ^= static_cast<Ty>(tail[3]) << 24;
                [[fallthrough]];
            case 3:
                k ^= static_cast<Ty>(tail[2]) << 16;
                [[fallthrough]];
            case 2:
                k ^= static_cast<Ty>(tail[1]) << 8;
                [[fallthrough]];
            case 1:
                k ^= static_cast<Ty>(tail[0]);
                k *= Parameters::c1;
                k = std::rotl(k, Parameters::r1);
                k *= Parameters::c2;
                h ^= k;
                break;
            }

            h ^= static_cast<Ty>(value.size());

            /// Fmix
            h ^= h >> Parameters::fmix_shift_1;
            h *= Parameters::fmix_c1;
            h ^= h >> Parameters::fmix_shift_2;
            h *= Parameters::fmix_c2;
            h ^= h >> Parameters::fmix_shift_3;

            return h;
        }
    };

    /// \brief Type alias for Murmur3 hash function
    using Murmur3_32 = Murmur3<std::uint32_t>;
} // namespace hashes

[[nodiscard]] inline consteval std::uint32_t operator""_murmur3_32(const char* value, std::size_t size) noexcept {
    return hashes::Murmur3_32::hash(std::span(value, size));
}
