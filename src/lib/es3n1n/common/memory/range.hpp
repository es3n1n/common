#pragma once
#include "address.hpp"

namespace memory {
    struct Range {
        Address start;
        Address end;

        [[nodiscard]] constexpr std::size_t size() const {
            return (end - start).as<std::size_t>();
        }
    };
} // namespace memory
