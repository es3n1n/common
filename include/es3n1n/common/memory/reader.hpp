#pragma once
#include <cstdint>
#include <cstring>
#include <expected>
#include <functional>
#include <optional>
#include <type_traits>

#include "es3n1n/common/base.hpp"
#include "es3n1n/common/traits.hpp"

namespace memory {
    /// \brief Error codes for memory operations
    enum class ErrorCode : std::uint8_t {
        UNKNOWN_ERROR = 0,
        INVALID_PARAMETERS, ///< Invalid parameters (sizes, most likely) were provided
        INVALID_ADDRESS,
        NOT_ENOUGH_BYTES,
    };

    using ReadPrimitive = std::expected<std::size_t, ErrorCode> (*)(void* buffer, std::uintptr_t address, std::size_t size);
    using WritePrimitive = std::expected<std::size_t, ErrorCode> (*)(std::uintptr_t address, const void* buffer, std::size_t size);

    namespace detail {
        inline std::optional<ErrorCode> sanitize_parameters(const void* buffer, const std::uintptr_t address, const std::size_t size) {
            if (buffer == nullptr || address == 0U) {
                return ErrorCode::INVALID_ADDRESS;
            }
            if (size == 0U) {
                return ErrorCode::INVALID_PARAMETERS;
            }
            return std::nullopt;
        }
    } // namespace detail

    inline std::expected<std::size_t, ErrorCode> default_read(void* buffer, const std::uintptr_t address, const std::size_t size) {
        if (auto err = detail::sanitize_parameters(buffer, address, size); err.has_value()) {
            return std::unexpected(err.value());
        }

        std::memcpy(buffer, reinterpret_cast<const void*>(address), size);
        return size;
    }

    inline std::expected<std::size_t, ErrorCode> default_write(const std::uintptr_t address, const void* buffer, const std::size_t size) {
        if (auto err = detail::sanitize_parameters(buffer, address, size); err.has_value()) {
            return std::unexpected(err.value());
        }

        std::memcpy(reinterpret_cast<void*>(address), buffer, size);
        return size;
    }

    /// \brief Memory reader implementation
    class Reader : public base::NonCopyable {
    public:
        constexpr Reader(): read_primitive_(default_read), write_primitive_(default_write) { }

        void read_primitive(ReadPrimitive read_func) {
            read_primitive_ = read_func;
        }

        void write_primitive(WritePrimitive write_func) {
            write_primitive_ = write_func;
        }

        std::expected<std::size_t, ErrorCode> read(void* buffer, const std::uintptr_t address, const std::size_t size) const {
            return read_primitive_(buffer, address, size);
        }

        template <traits::TriviallyCopyable Ty>
        std::expected<std::size_t, ErrorCode> read(Ty* dst, const std::uintptr_t src) const {
            return read(dst, src, sizeof(Ty));
        }

        template <traits::TriviallyCopyable Ty>
        std::expected<Ty, ErrorCode> read(const std::uintptr_t src) const {
            Ty obj = {};
            if (const auto res = read(&obj, src); !res.has_value()) {
                return std::unexpected(res.error());
            }
            return obj;
        }

        std::expected<std::size_t, ErrorCode> write(const std::uintptr_t address, const void* buffer, const std::size_t size) const {
            return write_primitive_(address, buffer, size);
        }

        template <traits::TriviallyCopyable Ty>
        std::expected<std::size_t, ErrorCode> write(const Ty* src, const std::uintptr_t dst) const {
            return write(dst, src, sizeof(Ty));
        }

    private:
        ReadPrimitive read_primitive_;
        WritePrimitive write_primitive_;
    };

    inline constinit auto reader = Reader();
} // namespace memory
