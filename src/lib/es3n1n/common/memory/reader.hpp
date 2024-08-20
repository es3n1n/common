#pragma once
#include <cstdint>
#include <expected>
#include <type_traits>

#include "es3n1n/common/traits.hpp"

namespace memory {
    enum class e_error_code : std::uint8_t {
        INVALID_PARAMETERS = 0,
    };

    inline std::expected<std::size_t, e_error_code> read(void* buffer, const std::uintptr_t address, const std::size_t size) {
        if (buffer == nullptr || address == 0U || size == 0U) {
            return std::unexpected(e_error_code::INVALID_PARAMETERS);
        }

        // NOLINTNEXTLINE
        std::memcpy(buffer, reinterpret_cast<void*>(address), size);
        return size;
    }

    template <traits::trivially_copyable Ty>
    std::expected<std::size_t, e_error_code> read(Ty* dst, const std::uintptr_t src) {
        return read(dst, src, sizeof(Ty));
    }

    template <traits::trivially_copyable Ty>
    std::expected<Ty, e_error_code> read(const std::uintptr_t src) {
        Ty _obj = {};

        if (const auto res = read(&_obj, src); !res.has_value()) {
            return std::unexpected(res.error());
        }

        return _obj;
    }

    inline std::expected<std::size_t, e_error_code> write(std::uintptr_t address, const void* buffer, const std::size_t size) {
        if (buffer == nullptr || address == 0U || size == 0U) {
            return std::unexpected(e_error_code::INVALID_PARAMETERS);
        }

        // NOLINTNEXTLINE
        std::memcpy(reinterpret_cast<void*>(address), buffer, size);
        return size;
    }

    template <traits::trivially_copyable Ty>
    std::expected<std::size_t, e_error_code> write(const Ty* src, const std::uintptr_t dst) {
        return write(dst, src, sizeof(Ty));
    }
} // namespace memory