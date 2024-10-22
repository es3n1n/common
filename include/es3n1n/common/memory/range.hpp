#pragma once
#include "address.hpp"

namespace memory {
    /// \brief Represents a contiguous range of memory.
    struct Range {
        Address start; ///< The starting address of the range.
        Address end; ///< The ending address of the range (exclusive).

        /// \brief Calculates the size of the range in bytes.
        /// \return The size of the range as a std::size_t.
        /// \note This method is marked as nodiscard to encourage using the returned value.
        [[nodiscard]] constexpr std::size_t size() const {
            return (end - start).as<std::size_t>();
        }

        /// \brief Checks if the range is empty.
        /// \return true if the range is empty (start == end), false otherwise.
        [[nodiscard]] constexpr bool is_empty() const {
            return start == end;
        }

        /// \brief Checks if a given address is within the range.
        /// \param address The address to check.
        /// \return true if the address is within the range, false otherwise.
        [[nodiscard]] constexpr bool contains(const Address& address) const {
            return address >= start && address < end;
        }

        /// \brief Checks if this range overlaps with another range.
        /// \param other The other range to check for overlap.
        /// \return true if the ranges overlap, false otherwise.
        [[nodiscard]] constexpr bool overlaps(const Range& other) const {
            return start < other.end && other.start < end;
        }
    };
} // namespace memory
