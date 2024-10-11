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
    template <traits::number Ty, std::size_t Rows, std::size_t Cols>
    class Matrix {
    public:
        /// \brief Default constructor.
        constexpr Matrix() = default;

        /// \brief Constructor from initializer list.
        /// \param init Nested initializer list representing the matrix elements.
        /// \throw std::invalid_argument If the dimensions of the initializer list do not match the matrix dimensions.
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

        /// \brief Access matrix element (mutable).
        /// \param row Row index.
        /// \param col Column index.
        /// \return Reference to the element at the specified position.
        [[nodiscard]] constexpr Ty& operator()(std::size_t row, std::size_t col) noexcept {
            return data_[row][col];
        }

        /// \brief Access matrix element (const).
        /// \param row Row index.
        /// \param col Column index.
        /// \return Const reference to the element at the specified position.
        [[nodiscard]] constexpr const Ty& operator()(std::size_t row, std::size_t col) const noexcept {
            return data_[row][col];
        }

        /// \brief Access matrix row (mutable).
        /// \param row Row index.
        /// \return Array representing the specified row.
        [[nodiscard]] constexpr std::array<Ty, Cols> operator[](std::size_t row) noexcept {
            return data_[row];
        }

        /// \brief Access matrix row (const).
        /// \param row Row index.
        /// \return Array representing the specified row.
        [[nodiscard]] constexpr const std::array<Ty, Cols> operator[](std::size_t row) const noexcept {
            return data_[row];
        }

        /// \brief Add another matrix to this matrix.
        /// \param other The matrix to add.
        /// \return Reference to this matrix after addition.
        constexpr Matrix& operator+=(const Matrix& other) noexcept {
            for (std::size_t i = 0; i < Rows; ++i) {
                std::transform(data_[i].begin(), data_[i].end(), other.data_[i].begin(), data_[i].begin(), std::plus<>());
            }
            return *this;
        }

        /// \brief Subtract another matrix from this matrix.
        /// \param other The matrix to subtract.
        /// \return Reference to this matrix after subtraction.
        constexpr Matrix& operator-=(const Matrix& other) noexcept {
            for (std::size_t i = 0; i < Rows; ++i) {
                std::transform(data_[i].begin(), data_[i].end(), other.data_[i].begin(), data_[i].begin(), std::minus<>());
            }
            return *this;
        }

        /// \brief Multiply this matrix by a scalar.
        /// \param scalar The scalar to multiply by.
        /// \return Reference to this matrix after multiplication.
        constexpr Matrix& operator*=(Ty scalar) noexcept {
            for (std::size_t i = 0; i < Rows; ++i) {
                std::transform(data_[i].begin(), data_[i].end(), data_[i].begin(), [scalar](Ty component) { return component * scalar; });
            }
            return *this;
        }

        /// \brief Divide this matrix by a scalar.
        /// \param scalar The scalar to divide by.
        /// \return Reference to this matrix after division.
        constexpr Matrix& operator/=(Ty scalar) noexcept {
            for (std::size_t i = 0; i < Rows; ++i) {
                std::transform(data_[i].begin(), data_[i].end(), data_[i].begin(), [scalar](Ty component) { return component / scalar; });
            }
            return *this;
        }

        /// \brief Add two matrices.
        /// \param other The matrix to add.
        /// \return Result of the addition.
        [[nodiscard]] constexpr Matrix operator+(const Matrix& other) const noexcept {
            Matrix result = *this;
            result += other;
            return result;
        }

        /// \brief Subtract two matrices.
        /// \param other The matrix to subtract.
        /// \return Result of the subtraction.
        [[nodiscard]] constexpr Matrix operator-(const Matrix& other) const noexcept {
            Matrix result = *this;
            result -= other;
            return result;
        }

        /// \brief Multiply matrix by scalar.
        /// \param scalar The scalar to multiply by.
        /// \return Result of the multiplication.
        [[nodiscard]] constexpr Matrix operator*(Ty scalar) const noexcept {
            Matrix result = *this;
            result *= scalar;
            return result;
        }

        /// \brief Divide matrix by scalar.
        /// \param scalar The scalar to divide by.
        /// \return Result of the division.
        [[nodiscard]] constexpr Matrix operator/(Ty scalar) const noexcept {
            Matrix result = *this;
            result /= scalar;
            return result;
        }

        /// \brief Multiply two matrices.
        /// \tparam OtherCols The number of columns in the other matrix.
        /// \param other The matrix to multiply with.
        /// \return Result of the matrix multiplication.
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

        /// \brief Multiply matrix by vector.
        /// \param vec The vector to multiply with.
        /// \return Result of the matrix-vector multiplication.
        [[nodiscard]] constexpr Vector<Ty, Rows> operator*(const Vector<Ty, Cols>& vec) const noexcept {
            Vector<Ty, Rows> result;
            for (std::size_t i = 0; i < Rows; ++i) {
                result[i] = std::transform_reduce(data_[i].begin(), data_[i].end(), vec.begin(), Ty{}, std::plus<>(), std::multiplies<>());
            }
            return result;
        }

        /// \brief Compare two matrices.
        /// \return Result of the comparison.
        [[nodiscard]] constexpr auto operator<=>(const Matrix&) const = default;

    private:
        std::array<std::array<Ty, Cols>, Rows> data_ = {}; ///< Matrix data
    };

    /// \brief Alias for 4x4 matrices.
    /// \tparam Ty The type of elements in the matrix (must be a number type).
    template <traits::number Ty>
    using mat4x4_t = Matrix<Ty, 4, 4>;

    /// \brief Alias for 3x4 matrices.
    /// \tparam Ty The type of elements in the matrix (must be a number type).
    template <traits::number Ty>
    using mat3x4_t = Matrix<Ty, 3, 4>;

    /// \brief Alias for 4x4 matrices of 32-bit floats.
    using mat4x4f32_t = mat4x4_t<float>;

    /// \brief Alias for 3x4 matrices of 32-bit floats.
    using mat3x4f32_t = mat3x4_t<float>;
} // namespace linalg
