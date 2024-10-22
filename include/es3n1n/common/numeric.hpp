#pragma once
#include <bit>
#include <concepts>
#include <cstdint>
#include <limits>
#include <utility>

namespace numeric {
    /// \brief A floating point range representation (from 0.F to 1.F)
    struct FloatRange {
        /* implicit */ constexpr FloatRange(const float value): value(value) { } // NOLINT(google-explicit-constructor, hicpp-explicit-conversions)
        explicit constexpr FloatRange(uint8_t) = delete;

        /* implicit */ constexpr operator float() const { // NOLINT(hicpp-explicit-conversions)
            return value;
        }

        float value = 0.F;
    };

    /// Backporting for older version of common lib
    using flt_range_t = FloatRange;

    template <std::integral Ty>
    [[nodiscard]] constexpr Ty to_le(const Ty value) noexcept {
        if constexpr (std::endian::native == std::endian::big) {
            return std::byteswap(value);
        } else {
            return value;
        }
    }

    template <std::integral Ty>
    [[nodiscard]] constexpr Ty to_be(const Ty value) noexcept {
        if constexpr (std::endian::native == std::endian::little) {
            return std::byteswap(value);
        } else {
            return value;
        }
    }

    template <std::integral Ty>
    [[nodiscard]] constexpr Ty to_endian(const Ty value, const std::endian endian) noexcept {
        switch (endian) {
        case std::endian::big:
            return to_be(value);
        case std::endian::little:
            return to_le(value);
        }
        std::unreachable();
    }
} // namespace numeric

template <>
class std::numeric_limits<numeric::flt_range_t> : public std::numeric_limits<float> {
public:
    [[nodiscard]] static constexpr numeric::flt_range_t(min)() noexcept {
        return 0.F;
    }

    [[nodiscard]] static constexpr numeric::flt_range_t(max)() noexcept {
        return 1.F;
    }
};
