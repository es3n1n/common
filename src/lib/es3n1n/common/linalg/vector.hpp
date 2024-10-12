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
    template <traits::number Ty, std::size_t N>
    class Vector {
    public:
        /// \brief Default constructor
        constexpr Vector() = default;

        /// \brief Constructor from initializer list
        /// \param init Initializer list containing vector components
        /// \throw std::invalid_argument if the initializer list size doesn't match the vector dimension
        constexpr Vector(std::initializer_list<Ty> init) {
            if (init.size() != N) [[unlikely]] {
                throw std::invalid_argument("Initializer list size does not match vector dimensions");
            }
            std::ranges::copy(init, components_.begin());
        }

        /// \brief Constructor from individual components
        /// \tparam TArgs Types of the input arguments
        /// \param args Individual component values
        template <typename... TArgs>
            requires(sizeof...(TArgs) == N)
        constexpr explicit Vector(TArgs... args): components_{static_cast<Ty>(args)...} { }

    public:
        /// \brief Access operator for vector components
        /// \param index Index of the component to access
        /// \return Reference to the component at the given index
        [[nodiscard]] constexpr Ty& operator[](std::size_t index) noexcept {
            return components_[index];
        }

        /// \brief Const access operator for vector components
        /// \param index Index of the component to access
        /// \return Const reference to the component at the given index
        [[nodiscard]] constexpr const Ty& operator[](std::size_t index) const noexcept {
            return components_[index];
        }

    public:
        /// \brief Calculate the magnitude (length) of the vector
        /// \return The magnitude of the vector
        [[nodiscard]] Ty magnitude() const noexcept {
            return std::sqrt(
                std::transform_reduce(components_.begin(), components_.end(), Ty{}, std::plus<>(), [](Ty component) { return component * component; }));
        }

        /// \brief Create a normalized version of the vector
        /// \return A new Vector object representing the normalized vector
        [[nodiscard]] Vector normalized() const noexcept {
            Vector result;
            Ty mag = magnitude();
            if (mag != 0) {
                std::transform(components_.begin(), components_.end(), result.components_.begin(), [mag](Ty component) { return component / mag; });
            }
            return result;
        }

        /// \brief Calculate the dot product with another vector
        /// \param other The vector to calculate the dot product with
        /// \return The dot product of the two vectors
        [[nodiscard]] Ty dot(const Vector& other) const noexcept {
            return std::inner_product(components_.begin(), components_.end(), other.components_.begin(), Ty{});
        }

        /// \brief Calculate the cross product with another vector (only for 3D vectors)
        /// \param other The vector to calculate the cross product with
        /// \return A new Vector object representing the cross product
        template <std::size_t M = N>
            requires(M == 3)
        [[nodiscard]] constexpr Vector cross(const Vector& other) const noexcept {
            return Vector{components_[1] * other[2] - components_[2] * other[1], //
                          components_[2] * other[0] - components_[0] * other[2], //
                          components_[0] * other[1] - components_[1] * other[0]};
        }

    public:
        /// \brief Addition assignment operator
        /// \param other The vector to add
        /// \return Reference to the modified vector
        constexpr Vector& operator+=(const Vector& other) noexcept {
            std::transform(components_.begin(), components_.end(), other.components_.begin(), components_.begin(), std::plus<>());
            return *this;
        }

        /// \brief Subtraction assignment operator
        /// \param other The vector to subtract
        /// \return Reference to the modified vector
        constexpr Vector& operator-=(const Vector& other) noexcept {
            std::transform(components_.begin(), components_.end(), other.components_.begin(), components_.begin(), std::minus<>());
            return *this;
        }

        /// \brief Scalar multiplication assignment operator
        /// \param scalar The scalar to multiply by
        /// \return Reference to the modified vector
        constexpr Vector& operator*=(Ty scalar) noexcept {
            std::transform(components_.begin(), components_.end(), components_.begin(), [scalar](Ty component) { return component * scalar; });
            return *this;
        }

        /// \brief Scalar division assignment operator
        /// \param scalar The scalar to divide by
        /// \return Reference to the modified vector
        constexpr Vector& operator/=(Ty scalar) noexcept {
            std::transform(components_.begin(), components_.end(), components_.begin(), [scalar](Ty component) { return component / scalar; });
            return *this;
        }

        /// \brief Three-way comparison operator
        [[nodiscard]] constexpr auto operator<=>(const Vector&) const = default;

        /// \brief Addition operator
        /// \param other The vector to add
        /// \return A new Vector object representing the sum
        [[nodiscard]] constexpr Vector operator+(const Vector& other) const noexcept {
            Vector result = *this;
            result += other;
            return result;
        }

        /// \brief Subtraction operator
        /// \param other The vector to subtract
        /// \return A new Vector object representing the difference
        [[nodiscard]] constexpr Vector operator-(const Vector& other) const noexcept {
            Vector result = *this;
            result -= other;
            return result;
        }

        /// \brief Scalar multiplication operator
        /// \param scalar The scalar to multiply by
        /// \return A new Vector object representing the product
        [[nodiscard]] constexpr Vector operator*(Ty scalar) const noexcept {
            Vector result = *this;
            result *= scalar;
            return result;
        }

        /// \brief Scalar division operator
        /// \param scalar The scalar to divide by
        /// \return A new Vector object representing the quotient
        [[nodiscard]] constexpr Vector operator/(Ty scalar) const noexcept {
            Vector result = *this;
            result /= scalar;
            return result;
        }

        /// \brief Getter for the x component (first component)
        /// \return The x component of the vector
        template <std::size_t M = N>
            requires(M >= 1)
        [[nodiscard]] constexpr Ty x() const noexcept {
            return components_[0];
        }

        /// \brief Getter for the y component (second component)
        /// \return The y component of the vector
        template <std::size_t M = N>
            requires(M >= 2)
        [[nodiscard]] constexpr Ty y() const noexcept {
            return components_[1];
        }

        /// \brief Getter for the z component (third component)
        /// \return The z component of the vector
        template <std::size_t M = N>
            requires(M >= 3)
        [[nodiscard]] constexpr Ty z() const noexcept {
            return components_[2];
        }

        /// \brief Getter for the w component (fourth component)
        /// \return The w component of the vector
        template <std::size_t M = N>
            requires(M >= 4)
        [[nodiscard]] constexpr Ty w() const noexcept {
            return components_[3];
        }

#if defined(COMMON_HAS_IMGUI)
        /// \brief Implicit conversion operator to ImVec2 (for 2D vectors)
        template <std::size_t M = N>
            requires(M == 2)
        /* implicit */ constexpr operator ImVec2() const noexcept {
            return {x(), y()};
        }

        /// \brief Implicit conversion operator to ImVec4 (for 4D vectors)
        template <std::size_t M = N>
            requires(M == 4)
        /* implicit */ constexpr operator ImVec4() const noexcept {
            return {x(), y(), z(), w()};
        }
#endif

        /// \brief Begin iterator for the vector components
        /// \return Iterator to the beginning of the vector components
        auto begin() {
            return components_.begin();
        }

        /// \brief End iterator for the vector components
        /// \return Iterator to the end of the vector components
        auto end() {
            return components_.end();
        }

        /// \brief Begin const iterator for the vector components
        /// \return Const iterator to the beginning of the vector components
        auto begin() const {
            return components_.begin();
        }

        /// \brief End iterator for the vector components
        /// \return Const iterator to the end of the vector components
        auto end() const {
            return components_.end();
        }

    private:
        /// \brief Array storing the vector components
        std::array<Ty, N> components_ = {}; ///< The vector components
    };

    /// \brief Type alias for 2D vector
    template <traits::number Ty>
    using Vector2 = Vector<Ty, 2>;

    /// \brief Type alias for 3D vector
    template <traits::number Ty>
    using Vector3 = Vector<Ty, 3>;

    /// \brief Type alias for 4D vector
    template <traits::number Ty>
    using Vector4 = Vector<Ty, 4>;

    /// \brief Type alias for 2D vector of unsigned 32-bit integers
    using Vector2u32 = Vector2<std::uint32_t>;
    /// \brief Type alias for 3D vector of unsigned 32-bit integers
    using Vector3u32 = Vector3<std::uint32_t>;
    /// \brief Type alias for 4D vector of unsigned 32-bit integers
    using Vector4u32 = Vector4<std::uint32_t>;

    /// \brief Type alias for 2D vector of signed 32-bit integers
    using Vector2i32 = Vector2<std::int32_t>;
    /// \brief Type alias for 3D vector of signed 32-bit integers
    using Vector3i32 = Vector3<std::int32_t>;
    /// \brief Type alias for 4D vector of signed 32-bit integers
    using Vector4i32 = Vector4<std::int32_t>;

    /// \brief Type alias for 2D vector of 32-bit floats
    using Vector2f32 = Vector2<float>;
    /// \brief Type alias for 3D vector of 32-bit floats
    using Vector3f32 = Vector3<float>;
    /// \brief Type alias for 4D vector of 32-bit floats
    using Vector4f32 = Vector4<float>;
} // namespace linalg
