#pragma once
#include <algorithm>
#include <array>
#include <cmath>
#include <concepts>
#include <cstdint>
#include <initializer_list>
#include <numeric>
#include <stdexcept>

#include "es3n1n/common/options.hpp"
#include "es3n1n/common/traits.hpp"

namespace linalg {
    /// \brief A templated vector class representing an N-dimensional vector of numeric type Ty
    /// \tparam Ty The numeric type of the vector components
    /// \tparam N The dimension of the vector
    template <traits::Number Ty, std::size_t N>
    class Vector {
    public:
        constexpr Vector() = default;

        constexpr Vector(std::initializer_list<Ty> init) {
            if (init.size() != N) [[unlikely]] {
                throw std::invalid_argument("Initializer list size does not match vector dimensions");
            }
            std::ranges::copy(init, components_.begin());
        }

        template <typename... TArgs>
            requires(sizeof...(TArgs) == N)
        constexpr explicit Vector(TArgs... args): components_{static_cast<Ty>(args)...} { }

    public:
        [[nodiscard]] constexpr Ty& operator[](std::size_t index) noexcept {
            return components_[index];
        }

        [[nodiscard]] constexpr const Ty& operator[](std::size_t index) const noexcept {
            return components_[index];
        }

    public:
        [[nodiscard]] Ty magnitude() const noexcept {
            return std::sqrt(
                std::transform_reduce(components_.begin(), components_.end(), Ty{}, std::plus<>(), [](Ty component) { return component * component; }));
        }

        [[nodiscard]] Vector normalized() const noexcept {
            Vector result;
            Ty mag = magnitude();
            if (mag != 0) {
                std::transform(components_.begin(), components_.end(), result.components_.begin(), [mag](Ty component) { return component / mag; });
            }
            return result;
        }

        [[nodiscard]] Ty dot(const Vector& other) const noexcept {
            return std::inner_product(components_.begin(), components_.end(), other.components_.begin(), Ty{});
        }

        template <std::size_t M = N>
            requires(M == 3)
        [[nodiscard]] constexpr Vector cross(const Vector& other) const noexcept {
            return Vector{components_[1] * other[2] - components_[2] * other[1], //
                          components_[2] * other[0] - components_[0] * other[2], //
                          components_[0] * other[1] - components_[1] * other[0]};
        }

    public:
        constexpr Vector& operator+=(const Vector& other) noexcept {
            std::transform(components_.begin(), components_.end(), other.components_.begin(), components_.begin(), std::plus<>());
            return *this;
        }

        constexpr Vector& operator-=(const Vector& other) noexcept {
            std::transform(components_.begin(), components_.end(), other.components_.begin(), components_.begin(), std::minus<>());
            return *this;
        }

        constexpr Vector& operator*=(Ty scalar) noexcept {
            std::transform(components_.begin(), components_.end(), components_.begin(), [scalar](Ty component) { return component * scalar; });
            return *this;
        }

        constexpr Vector& operator/=(Ty scalar) noexcept {
            std::transform(components_.begin(), components_.end(), components_.begin(), [scalar](Ty component) { return component / scalar; });
            return *this;
        }

        [[nodiscard]] constexpr auto operator<=>(const Vector&) const = default;

        [[nodiscard]] constexpr Vector operator+(const Vector& other) const noexcept {
            Vector result = *this;
            result += other;
            return result;
        }

        [[nodiscard]] constexpr Vector operator-(const Vector& other) const noexcept {
            Vector result = *this;
            result -= other;
            return result;
        }

        [[nodiscard]] constexpr Vector operator*(Ty scalar) const noexcept {
            Vector result = *this;
            result *= scalar;
            return result;
        }

        [[nodiscard]] constexpr Vector operator/(Ty scalar) const noexcept {
            Vector result = *this;
            result /= scalar;
            return result;
        }

        template <std::size_t M = N>
            requires(M >= 1)
        [[nodiscard]] constexpr Ty x() const noexcept {
            return components_[0];
        }

        template <std::size_t M = N>
            requires(M >= 2)
        [[nodiscard]] constexpr Ty y() const noexcept {
            return components_[1];
        }

        template <std::size_t M = N>
            requires(M >= 3)
        [[nodiscard]] constexpr Ty z() const noexcept {
            return components_[2];
        }

        template <std::size_t M = N>
            requires(M >= 4)
        [[nodiscard]] constexpr Ty w() const noexcept {
            return components_[3];
        }

#if defined(COMMON_HAS_IMGUI)
        template <std::size_t M = N>
            requires(M == 2)
        /* implicit */ constexpr operator ImVec2() const noexcept {
            return {x(), y()};
        }

        template <std::size_t M = N>
            requires(M == 4)
        /* implicit */ constexpr operator ImVec4() const noexcept {
            return {x(), y(), z(), w()};
        }
#endif

        auto begin() {
            return components_.begin();
        }

        auto end() {
            return components_.end();
        }

        auto begin() const {
            return components_.begin();
        }

        auto end() const {
            return components_.end();
        }

    private:
        std::array<Ty, N> components_ = {};
    };

    template <traits::Number Ty>
    using Vector2 = Vector<Ty, 2>;
    template <traits::Number Ty>
    using Vector3 = Vector<Ty, 3>;
    template <traits::Number Ty>
    using Vector4 = Vector<Ty, 4>;

    using Vector2u32 = Vector2<std::uint32_t>;
    using Vector3u32 = Vector3<std::uint32_t>;
    using Vector4u32 = Vector4<std::uint32_t>;

    using Vector2i32 = Vector2<std::int32_t>;
    using Vector3i32 = Vector3<std::int32_t>;
    using Vector4i32 = Vector4<std::int32_t>;

    using Vector2f32 = Vector2<float>;
    using Vector3f32 = Vector3<float>;
    using Vector4f32 = Vector4<float>;
} // namespace linalg
