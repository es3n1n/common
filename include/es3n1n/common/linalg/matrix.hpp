#pragma once
#include <algorithm>
#include <array>
#include <functional>
#include <initializer_list>
#include <stdexcept>

#include "es3n1n/common/linalg/vector.hpp"
#include "es3n1n/common/traits.hpp"

namespace linalg {
    /// \brief A template class representing a matrix of fixed size.
    /// \tparam Ty The type of elements in the matrix (must be a number type).
    /// \tparam Rows The number of rows in the matrix.
    /// \tparam Cols The number of columns in the matrix.
    template <traits::Number Ty, std::size_t Rows, std::size_t Cols>
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

        [[nodiscard]] constexpr auto operator<=>(const Matrix&) const = default;

    private:
        std::array<std::array<Ty, Cols>, Rows> data_ = {};
    };

    template <traits::Number Ty>
    using Matrix4x4 = Matrix<Ty, 4, 4>;
    template <traits::Number Ty>
    using Matrix3x4 = Matrix<Ty, 3, 4>;
    using Matrix4x4f32 = Matrix4x4<float>;
    using Matrix3x4f32 = Matrix3x4<float>;
} // namespace linalg