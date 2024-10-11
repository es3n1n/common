#pragma once
#include <algorithm>
#include <array>
#include <functional>
#include <initializer_list>
#include <stdexcept>

#include "es3n1n/common/linalg/vector.hpp"
#include "es3n1n/common/traits.hpp"

namespace linalg {
    template <traits::number Ty, std::size_t Rows, std::size_t Cols>
    class Matrix {
    public:
        constexpr Matrix() = default;

        constexpr Matrix(std::initializer_list<std::initializer_list<Ty>> init) {
            if (init.size() != Rows) [[unlikely]] {
                throw std::invalid_argument("Initializer list row count does not match matrix dimensions");
            }
            auto row_it = init.begin();
            for (std::size_t i = 0; i < Rows; ++i, ++row_it) {
                if (row_it->size() != Cols) [[unlikely]] {
                    throw std::invalid_argument("Initializer list column count does not match matrix dimensions");
                }
                std::ranges::copy(*row_it, data_[i].begin());
            }
        }

    public:
        [[nodiscard]] constexpr Ty& operator()(std::size_t row, std::size_t col) noexcept {
            return data_[row][col];
        }

        [[nodiscard]] constexpr const Ty& operator()(std::size_t row, std::size_t col) const noexcept {
            return data_[row][col];
        }

        [[nodiscard]] constexpr std::array<Ty, Cols> operator[](std::size_t row) noexcept {
            return data_[row];
        }

        [[nodiscard]] constexpr const std::array<Ty, Cols> operator[](std::size_t row) const noexcept {
            return data_[row];
        }

    public:
        constexpr Matrix& operator+=(const Matrix& other) noexcept {
            for (std::size_t i = 0; i < Rows; ++i) {
                std::transform(data_[i].begin(), data_[i].end(), other.data_[i].begin(), data_[i].begin(), std::plus<>());
            }
            return *this;
        }

        constexpr Matrix& operator-=(const Matrix& other) noexcept {
            for (std::size_t i = 0; i < Rows; ++i) {
                std::transform(data_[i].begin(), data_[i].end(), other.data_[i].begin(), data_[i].begin(), std::minus<>());
            }
            return *this;
        }

        constexpr Matrix& operator*=(Ty scalar) noexcept {
            for (std::size_t i = 0; i < Rows; ++i) {
                std::transform(data_[i].begin(), data_[i].end(), data_[i].begin(), [scalar](Ty component) { return component * scalar; });
            }
            return *this;
        }

        constexpr Matrix& operator/=(Ty scalar) noexcept {
            for (std::size_t i = 0; i < Rows; ++i) {
                std::transform(data_[i].begin(), data_[i].end(), data_[i].begin(), [scalar](Ty component) { return component / scalar; });
            }
            return *this;
        }

    public:
        [[nodiscard]] constexpr Matrix operator+(const Matrix& other) const noexcept {
            Matrix result = *this;
            result += other;
            return result;
        }

        [[nodiscard]] constexpr Matrix operator-(const Matrix& other) const noexcept {
            Matrix result = *this;
            result -= other;
            return result;
        }

        [[nodiscard]] constexpr Matrix operator*(Ty scalar) const noexcept {
            Matrix result = *this;
            result *= scalar;
            return result;
        }

        [[nodiscard]] constexpr Matrix operator/(Ty scalar) const noexcept {
            Matrix result = *this;
            result /= scalar;
            return result;
        }

        template <std::size_t OtherCols>
        [[nodiscard]] constexpr Matrix<Ty, Rows, OtherCols> operator*(const Matrix<Ty, Cols, OtherCols>& other) const noexcept {
            Matrix<Ty, Rows, OtherCols> result;
            for (std::size_t i = 0; i < Rows; ++i) {
                for (std::size_t j = 0; j < OtherCols; ++j) {
                    result(i, j) = Ty{};
                    for (std::size_t k = 0; k < Cols; ++k) {
                        result(i, j) += data_[i][k] * other(k, j);
                    }
                }
            }
            return result;
        }

        [[nodiscard]] constexpr Vector<Ty, Rows> operator*(const Vector<Ty, Cols>& vec) const noexcept {
            Vector<Ty, Rows> result;
            for (std::size_t i = 0; i < Rows; ++i) {
                result[i] = std::transform_reduce(data_[i].begin(), data_[i].end(), vec.begin(), Ty{}, std::plus<>(), std::multiplies<>());
            }
            return result;
        }

    public:
        [[nodiscard]] constexpr auto operator<=>(const Matrix&) const = default;

    private:
        std::array<std::array<Ty, Cols>, Rows> data_ = {};
    };

    /// Aliases for 4x4 and 3x4 matrices
    template <traits::number Ty>
    using mat4x4_t = Matrix<Ty, 4, 4>;

    template <traits::number Ty>
    using mat3x4_t = Matrix<Ty, 3, 4>;

    /// Commonly used matrix types
    using mat4x4f32_t = mat4x4_t<float>;
    using mat3x4f32_t = mat3x4_t<float>;
} // namespace linalg
