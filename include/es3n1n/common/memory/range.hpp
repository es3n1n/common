#pragma once
#include "address.hpp"

namespace memory {
    /// \brief Represents a contiguous range of memory.
    struct Range {
        Address start;
        Address end; ///< The ending address of the range (exclusive).

        [[nodiscard]] constexpr std::size_t size() const {
            return (end - start).as<std::size_t>();
        }

        [[nodiscard]] constexpr bool is_empty() const {
            return start == end;
        }

        [[nodiscard]] constexpr bool contains(const Address& address) const {
            return address >= start && address < end;
        }

        [[nodiscard]] constexpr bool overlaps(const Range& other) const {
            return start < other.end && other.start < end;
        }
    };
} // namespace memory
