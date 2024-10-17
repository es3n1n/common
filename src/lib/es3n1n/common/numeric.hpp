#pragma once
#include <bit>
#include <concepts>
#include <cstdint>
#include <limits>
#include <utility>

namespace numeric {
    /// \brief A floating point range representation (from 0.F to 1.F)
    struct flt_range_t {
        /// \brief Init a range with desired value
        /// \param value a value in ranges (0.F to 1.F)
        /* implicit */ constexpr flt_range_t(const float value): value(value) { } // NOLINT(google-explicit-constructor, hicpp-explicit-conversions)

        /// \brief Prevent implicit conversion from uint8_t
        explicit constexpr flt_range_t(uint8_t value) = delete;

        /// \brief Implicitly convert range to the float
        /// \return floating point value from 0.F to 1.F
        /* implicit */ constexpr operator float() const { // NOLINT(google-explicit-constructor, hicpp-explicit-conversions)
            return value;
        }

        float value = 0.F;
    };

    /// \brief Convert a value to little-endian
    /// \tparam Ty The integral type
    /// \param value The value to convert
    /// \return The value in little-endian
    template <std::integral Ty>
    [[nodiscard]] constexpr Ty to_le(const Ty value) noexcept {
        if constexpr (std::endian::native == std::endian::big) {
            return std::byteswap(value);
        } else {
            return value;
        }
    }

    /// \brief Convert a value to big-endian
    /// \tparam Ty The integral type
    /// \param value The value to convert
    /// \return The value in big-endian
    template <std::integral Ty>
    [[nodiscard]] constexpr Ty to_be(const Ty value) noexcept {
        if constexpr (std::endian::native == std::endian::little) {
            return std::byteswap(value);
        } else {
            return value;
        }
    }

    /// \brief Convert a value to the desired endianness
    /// \tparam Ty The integral type
    /// \param value The value to convert
    /// \param endian The desired endianness
    /// \return The value in the desired endianness
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
    /// \brief Get a minimal range value
    /// \return 0.F
    [[nodiscard]] static constexpr numeric::flt_range_t(min)() noexcept {
        return 0.F;
    }

    /// \brief Get a maximal range value
    /// \return 1.F
    [[nodiscard]] static constexpr numeric::flt_range_t(max)() noexcept {
        return 1.F;
    }
};
