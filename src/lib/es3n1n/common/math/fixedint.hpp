#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <format>
#include <string>

#include "es3n1n/common/memory/size.hpp"

namespace math {
    enum class BigIntSerializationMode : std::uint8_t {
        HEX_LOWERCASE = 0,
        HEX_UPPERCASE,
    };

    template <std::size_t Bits>
        requires(Bits > *memory::kWordInBits && Bits % CHAR_BIT == 0)
    class FixedInt {
    public:
        constexpr FixedInt() = default;
        ~FixedInt() = default;

        template <std::unsigned_integral Ty>
        constexpr explicit FixedInt(Ty value) {
            data_[0] = static_cast<memory::Word>(value);
        }

        [[nodiscard]] constexpr auto operator<=>(const FixedInt&) const = default;

        constexpr FixedInt& operator*=(const FixedInt& rhs) {
            FixedInt result;

            for (std::size_t i = 0; i < kNumberOfWords; ++i) {
                memory::Word carry = 0;
                for (std::size_t j = 0; j < kNumberOfWords; ++j) {
                    auto a = data_[i];
                    auto b = rhs.data_[j];

                    auto a_hi = a >> *memory::kHalfWordInBits;
                    auto a_lo = a & ((1ULL << *memory::kHalfWordInBits) - 1);
                    auto b_hi = b >> (*memory::kWordInBits / 2);
                    auto b_lo = b & ((1ULL << *memory::kHalfWordInBits) - 1);

                    auto lo_lo = a_lo * b_lo;
                    auto hi_lo = a_hi * b_lo;
                    auto lo_hi = a_lo * b_hi;
                    auto hi_hi = a_hi * b_hi;

                    auto lo_product = lo_lo + (data_[i + j] + carry);
                    carry = lo_product < lo_lo;

                    auto mid1 = (lo_product >> *memory::kHalfWordInBits) + hi_lo + lo_hi;
                    carry += mid1 < hi_lo;

                    auto hi_product = (mid1 >> *memory::kHalfWordInBits) + hi_hi;
                    carry += hi_product < hi_hi;

                    result.data_[i + j] = lo_product & ((1ULL << *memory::kWordInBits) - 1);
                    carry += hi_product;
                }

                // result.data_[i + kNumberOfWords] += carry;
            }

            *this = result;
            return *this;
        }

        [[nodiscard]] constexpr std::string to_string(const BigIntSerializationMode mode = BigIntSerializationMode::HEX_LOWERCASE) const {
            std::string result;

            for (auto& word : data_) {
                switch (mode) {
                case BigIntSerializationMode::HEX_LOWERCASE:
                    result += std::format("{:0{}x}", word, kNumberOfHexDigitsPerWord);
                    break;
                case BigIntSerializationMode::HEX_UPPERCASE:
                    result += std::format("{:0{}X}", word, kNumberOfHexDigitsPerWord);
                    break;
                }
            }

            return result;
        }

    private:
        constexpr static auto kNumberOfWords = (Bits + (*memory::kWordInBits - 1)) / *memory::kWordInBits;
        constexpr static auto kNumberOfHexDigitsPerWord = *(memory::kWordInBits / 4_bits);

        std::array<memory::Word, kNumberOfWords * 2> data_ = {};
    };

    using Int128 = FixedInt<128>;
    using Int256 = FixedInt<256>;
} // namespace math
