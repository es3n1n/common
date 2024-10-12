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
    enum class ErrorCode : std::uint8_t {
        UNKNOWN_ERROR = 0,
        INVALID_PARAMETERS,
        INVALID_ADDRESS,
        NOT_ENOUGH_BYTES,
    };

    // Not std::function in favor of constexpr-ness
    using ReadPrimitive = std::expected<std::size_t, ErrorCode> (*)(void*, std::uintptr_t, std::size_t);
    using WritePrimitive = std::expected<std::size_t, ErrorCode> (*)(std::uintptr_t, const void*, std::size_t);

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

    /// Default read primitive
    /// \param buffer output buffer pointer
    /// \param address source address
    /// \param size number of bytes to read
    /// \return number of bytes read on success, error code on failure
    inline std::expected<std::size_t, ErrorCode> default_read(void* buffer, const std::uintptr_t address, const std::size_t size) {
        if (auto err = detail::sanitize_parameters(buffer, address, size); err.has_value()) {
            return std::unexpected(err.value());
        }

        // NOLINTNEXTLINE
        std::memcpy(buffer, reinterpret_cast<void*>(address), size);
        return size;
    }

    /// Default write primitive
    /// \param address destination address
    /// \param buffer buffer of data to write
    /// \param size number of bytes to write
    /// \return number of bytes written on success, error code on failure
    inline std::expected<std::size_t, ErrorCode> default_write(std::uintptr_t address, const void* buffer, const std::size_t size) {
        if (auto err = detail::sanitize_parameters(buffer, address, size); err.has_value()) {
            return std::unexpected(err.value());
        }

        // NOLINTNEXTLINE
        std::memcpy(reinterpret_cast<void*>(address), buffer, size);
        return size;
    }

    /// Memory reader implementation
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

        template <traits::trivially_copyable Ty>
        std::expected<std::size_t, ErrorCode> read(Ty* dst, const std::uintptr_t src) const {
            return read(dst, src, sizeof(Ty));
        }

        template <traits::trivially_copyable Ty>
        std::expected<Ty, ErrorCode> read(const std::uintptr_t src) const {
            Ty _obj = {};

            if (const auto res = read(&_obj, src); !res.has_value()) {
                return std::unexpected(res.error());
            }

            return _obj;
        }

        std::expected<std::size_t, ErrorCode> write(std::uintptr_t address, const void* buffer, const std::size_t size) const {
            return write_primitive_(address, buffer, size);
        }

        template <traits::trivially_copyable Ty>
        std::expected<std::size_t, ErrorCode> write(const Ty* src, const std::uintptr_t dst) const {
            return write(dst, src, sizeof(Ty));
        }

    private:
        ReadPrimitive read_primitive_;
        WritePrimitive write_primitive_;
    };

    /// Shared reader instance
    inline constinit auto reader = Reader();
} // namespace memory
