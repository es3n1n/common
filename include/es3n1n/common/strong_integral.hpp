#pragma once
#include <compare>
#include <concepts>
#include <type_traits>

namespace types {
    template <std::integral Ty, typename Tag> /// Tag is not unused!
    class StrongIntegral {
    public:
        constexpr StrongIntegral() = default;
        explicit constexpr StrongIntegral(Ty value): value_(value) { }

        [[nodiscard]] constexpr Ty value() const {
            return value_;
        }

        explicit constexpr operator Ty() const {
            return value_;
        }

        constexpr StrongIntegral& operator++() {
            ++value_;
            return *this;
        }

        constexpr StrongIntegral& operator--() {
            --value_;
            return *this;
        }

        constexpr StrongIntegral operator++(int) {
            auto tmp = *this;
            ++*this;
            return tmp;
        }

        constexpr StrongIntegral operator--(int) {
            auto tmp = *this;
            --*this;
            return tmp;
        }

        constexpr StrongIntegral& operator+=(const StrongIntegral& rhs) {
            value_ += rhs.value_;
            return *this;
        }

        constexpr StrongIntegral& operator-=(const StrongIntegral& rhs) {
            value_ -= rhs.value_;
            return *this;
        }

        constexpr StrongIntegral& operator*=(const StrongIntegral& rhs) {
            value_ *= rhs.value_;
            return *this;
        }

        constexpr StrongIntegral& operator/=(const StrongIntegral& rhs) {
            value_ /= rhs.value_;
            return *this;
        }

        constexpr StrongIntegral& operator%=(const StrongIntegral& rhs) {
            value_ %= rhs.value_;
            return *this;
        }

        constexpr auto operator<=>(const StrongIntegral&) const = default;

        constexpr StrongIntegral operator+() const {
            return StrongIntegral{+value_};
        }

        constexpr StrongIntegral operator-() const {
            return StrongIntegral{-value_};
        }

        constexpr StrongIntegral operator~() const {
            return StrongIntegral{~value_};
        }

        constexpr StrongIntegral operator+(const StrongIntegral& rhs) const {
            return StrongIntegral{value_ + rhs.value_};
        }

        constexpr StrongIntegral operator-(const StrongIntegral& rhs) const {
            return StrongIntegral{value_ - rhs.value_};
        }

        constexpr StrongIntegral operator*(const StrongIntegral& rhs) const {
            return StrongIntegral{value_ * rhs.value_};
        }

        constexpr StrongIntegral operator/(const StrongIntegral& rhs) const {
            return StrongIntegral{value_ / rhs.value_};
        }

        constexpr StrongIntegral operator%(const StrongIntegral& rhs) const {
            return StrongIntegral{value_ % rhs.value_};
        }

        constexpr StrongIntegral operator&(const StrongIntegral& rhs) const {
            return StrongIntegral{value_ & rhs.value_};
        }

        constexpr StrongIntegral operator|(const StrongIntegral& rhs) const {
            return StrongIntegral{value_ | rhs.value_};
        }

        constexpr StrongIntegral operator^(const StrongIntegral& rhs) const {
            return StrongIntegral{value_ ^ rhs.value_};
        }

        constexpr StrongIntegral operator<<(int shift) const {
            return StrongIntegral{value_ << shift};
        }

        constexpr StrongIntegral operator>>(int shift) const {
            return StrongIntegral{value_ >> shift};
        }

    private:
        Ty value_ = {};
    };
} // namespace types
