#pragma once
#include "es3n1n/common/hashes/base.hpp"
#include "es3n1n/common/numeric.hpp"
#include <array>
#include <climits>

namespace hashes {
    namespace detail {
        template <detail::HashSize Ty, std::endian Endian = std::endian::little>
        struct MurmurParameters {
            static constexpr std::endian endian = Endian;

            static constexpr Ty c1 = sizeof(Ty) == 4 ? 0xcc9e2d51 : 0x87c37b91114253d5;
            static constexpr Ty c2 = sizeof(Ty) == 4 ? 0x1b873593 : 0x4cf5ad432745937f;

            static constexpr Ty r1 = sizeof(Ty) == 4 ? 15 : 31;
            static constexpr Ty r2 = sizeof(Ty) == 4 ? 13 : 27;
            static constexpr Ty m = 5;
            static constexpr Ty n = 0xe6546b64;

            static constexpr Ty fmix_c1 = sizeof(Ty) == 4 ? 0x85ebca6b : 0xff51afd7ed558ccd;
            static constexpr Ty fmix_c2 = sizeof(Ty) == 4 ? 0xc2b2ae35 : 0xc4ceb9fe1a85ec53;
            static constexpr Ty fmix_shift_1 = sizeof(Ty) == 4 ? 16 : 33;
            static constexpr Ty fmix_shift_2 = sizeof(Ty) == 4 ? 13 : 33;
            static constexpr Ty fmix_shift_3 = sizeof(Ty) == 4 ? 16 : 33;
        };
    } // namespace detail

    /// \brief Murmur3 hash function
    /// \tparam Ty The size type for the hash
    /// \note This implementation hashes the sequence of bytes,
    ///     so hash(L"hello") and hash("hello") will produce different results
    /// \see https://en.wikipedia.org/wiki/MurmurHash
    template <detail::HashSize Ty, Ty Seed = 0, typename Parameters = detail::MurmurParameters<Ty>>
        requires(std::is_same_v<Ty, std::uint32_t>) /// \todo @es3n1n: Add support for 128-bit hashes once we have a 128-bit integer type
    class Murmur3 : public HashFunction<Murmur3<Ty>, Ty> {
        template <detail::Hashable CharTy>
        [[nodiscard]] static constexpr Ty read_imm(const std::span<CharTy>& value, const std::size_t offset) noexcept {
            Ty result = 0;
            const std::size_t remaining = value.size() - offset;
            const std::size_t bytes_to_read = std::min(sizeof(Ty), remaining * sizeof(CharTy));

            for (std::size_t i = 0; i < bytes_to_read; ++i) {
                const std::size_t char_index = offset + (i / sizeof(CharTy));
                const std::size_t byte_index = i % sizeof(CharTy);

                const auto char_val = std::bit_cast<std::make_unsigned_t<CharTy>>(value[char_index]);
                const auto shift_count = byte_index * CHAR_BIT;
                const auto byte = static_cast<std::uint8_t>((char_val >> shift_count) & 0xFFU);
                result |= static_cast<Ty>(byte) << (i * CHAR_BIT);
            }

            return numeric::to_endian(result, Parameters::endian);
        }

    public:
        template <detail::Hashable CharTy>
        [[nodiscard]] static constexpr Ty hash_impl(const std::span<CharTy> value) noexcept {
            Ty h = Seed;

            const std::size_t len = value.size() * sizeof(CharTy);
            const std::size_t num_blocks = len / sizeof(Ty);

            for (std::size_t i = 0; i < num_blocks; ++i) {
                auto k = read_imm(value, i * sizeof(Ty) / sizeof(CharTy));

                k *= Parameters::c1;
                k = std::rotl(k, Parameters::r1);
                k *= Parameters::c2;

                h ^= k;
                h = std::rotl(h, Parameters::r2);
                h = h * Parameters::m + Parameters::n;
            }

            const auto tail = value.subspan(num_blocks * sizeof(Ty) / sizeof(CharTy));
            Ty k = 0;

            if (!tail.empty()) {
                for (std::size_t i = 0; i < tail.size(); ++i) {
                    k ^= static_cast<Ty>(tail[i]) << (i * sizeof(CharTy) * CHAR_BIT);
                }
                k *= Parameters::c1;
                k = std::rotl(k, Parameters::r1);
                k *= Parameters::c2;
                h ^= k;
            }

            h ^= static_cast<Ty>(len);

            h ^= h >> Parameters::fmix_shift_1;
            h *= Parameters::fmix_c1;
            h ^= h >> Parameters::fmix_shift_2;
            h *= Parameters::fmix_c2;
            h ^= h >> Parameters::fmix_shift_3;
            return h;
        }
    };

    using Murmur3_32 = Murmur3<std::uint32_t>;
} // namespace hashes

[[nodiscard]] consteval std::uint32_t operator""_murmur3_32(const char* value, std::size_t size) noexcept {
    return hashes::Murmur3_32::hash(std::span(value, size));
}
