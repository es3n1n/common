#pragma once
#include <compare>
#include <concepts>
#include <type_traits>

namespace types {
    /// \brief A template for creating strong integral types
    /// \tparam T The underlying integral type
    /// \tparam Tag A tag type for uniqueness
    template <std::integral Ty, typename Tag>
    class StrongIntegral {
    public:
        /// \brief The underlying type of the strong integral
        using UnderlyingType = Ty;

        /// \brief Default constructor
        constexpr StrongIntegral() = default;

        /// \brief Constructor from the underlying type
        /// \param value The value to initialize the strong integral with
        explicit constexpr StrongIntegral(Ty value): value_(value) { }

        /// \brief Get the underlying value
        /// \return The underlying value
        [[nodiscard]] constexpr Ty value() const {
            return value_;
        }

        /// \brief Explicitly convert to the underlying type
        /// \return The underlying value
        explicit constexpr operator Ty() const {
            return value_;
        }

        /// \brief Increment operator
        /// \return Reference to the incremented object
        constexpr StrongIntegral& operator++() {
            ++value_;
            return *this;
        }

        /// \brief Decrement operator
        /// \return Reference to the decremented object
        constexpr StrongIntegral& operator--() {
            --value_;
            return *this;
        }

        /// \brief Postfix increment operator
        /// \return Copy of the object before incrementing
        constexpr StrongIntegral operator++(int) {
            auto tmp = *this;
            ++*this;
            return tmp;
        }

        /// \brief Postfix decrement operator
        /// \return Copy of the object before decrementing
        constexpr StrongIntegral operator--(int) {
            auto tmp = *this;
            --*this;
            return tmp;
        }

        /// \brief Addition assignment operator
        /// \param rhs The right-hand side operand
        /// \return Reference to the result of the addition
        constexpr StrongIntegral& operator+=(const StrongIntegral& rhs) {
            value_ += rhs.value_;
            return *this;
        }

        /// \brief Subtraction assignment operator
        /// \param rhs The right-hand side operand
        /// \return Reference to the result of the subtraction
        constexpr StrongIntegral& operator-=(const StrongIntegral& rhs) {
            value_ -= rhs.value_;
            return *this;
        }

        /// \brief Multiplication assignment operator
        /// \param rhs The right-hand side operand
        /// \return Reference to the result of the multiplication
        constexpr StrongIntegral& operator*=(const StrongIntegral& rhs) {
            value_ *= rhs.value_;
            return *this;
        }

        /// \brief Division assignment operator
        /// \param rhs The right-hand side operand
        /// \return Reference to the result of the division
        constexpr StrongIntegral& operator/=(const StrongIntegral& rhs) {
            value_ /= rhs.value_;
            return *this;
        }

        /// \brief Modulo assignment operator
        /// \param rhs The right-hand side operand
        /// \return Reference to the result of the modulo operation
        constexpr StrongIntegral& operator%=(const StrongIntegral& rhs) {
            value_ %= rhs.value_;
            return *this;
        }

        /// \brief Spaceship operator for three-way comparison
        /// \param rhs The right-hand side operand
        /// \return The result of the three-way comparison
        constexpr auto operator<=>(const StrongIntegral&) const = default;

    private:
        Ty value_ = {}; ///< The underlying value
    };
} // namespace types
