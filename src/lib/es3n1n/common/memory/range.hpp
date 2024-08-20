#pragma once
#include "address.hpp"

namespace memory {
    using rva_t = memory::address;

    struct range_t {
        rva_t start;
        rva_t end;

        [[nodiscard]] constexpr std::size_t size() const {
            return (end - start).as<std::size_t>();
        }
    };
} // namespace memory
