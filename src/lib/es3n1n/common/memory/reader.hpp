#pragma once
#include <cstdint>
#include <expected>
#include <functional>
#include <type_traits>

#include "es3n1n/common/base.hpp"
#include "es3n1n/common/traits.hpp"

namespace memory {
    enum class e_error_code : std::uint8_t {
        INVALID_PARAMETERS = 0,
        INVALID_ADDRESS,
        NOT_ENOUGH_BYTES,
    };

    // Not std::function in favor of constexpr-ness
    using read_primitive_t = std::expected<std::size_t, e_error_code> (*)(void*, std::uintptr_t, std::size_t);
    using write_primitive_t = std::expected<std::size_t, e_error_code> (*)(std::uintptr_t, const void*, std::size_t);

    /// Default read primitive
    /// \param buffer output buffer pointer
    /// \param address source address
    /// \param size number of bytes to read
    /// \return number of bytes read on success, error code on failure
    inline std::expected<std::size_t, e_error_code> default_read(void* buffer, const std::uintptr_t address, const std::size_t size) {
        if (buffer == nullptr || address == 0U || size == 0U) {
            return std::unexpected(e_error_code::INVALID_PARAMETERS);
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
    inline std::expected<std::size_t, e_error_code> default_write(std::uintptr_t address, const void* buffer, const std::size_t size) {
        if (buffer == nullptr || address == 0U || size == 0U) {
            return std::unexpected(e_error_code::INVALID_PARAMETERS);
        }

        // NOLINTNEXTLINE
        std::memcpy(reinterpret_cast<void*>(address), buffer, size);
        return size;
    }

    /// Memory reader implementation
    class Reader : public base::NonCopyable {
    public:
        constexpr Reader(): m_read_primitive_(default_read), m_write_primitive_(default_write) { }

        void read_primitive(read_primitive_t read_func) {
            m_read_primitive_ = read_func;
        }

        void write_primitive(write_primitive_t write_func) {
            m_write_primitive_ = write_func;
        }

        std::expected<std::size_t, e_error_code> read(void* buffer, const std::uintptr_t address, const std::size_t size) const {
            return m_read_primitive_(buffer, address, size);
        }

        template <traits::trivially_copyable Ty>
        std::expected<std::size_t, e_error_code> read(Ty* dst, const std::uintptr_t src) const {
            return read(dst, src, sizeof(Ty));
        }

        template <traits::trivially_copyable Ty>
        std::expected<Ty, e_error_code> read(const std::uintptr_t src) const {
            Ty _obj = {};

            if (const auto res = read(&_obj, src); !res.has_value()) {
                return std::unexpected(res.error());
            }

            return _obj;
        }

        std::expected<std::size_t, e_error_code> write(std::uintptr_t address, const void* buffer, const std::size_t size) const {
            return m_write_primitive_(address, buffer, size);
        }

        template <traits::trivially_copyable Ty>
        std::expected<std::size_t, e_error_code> write(const Ty* src, const std::uintptr_t dst) const {
            return write(dst, src, sizeof(Ty));
        }

    private:
        read_primitive_t m_read_primitive_;
        write_primitive_t m_write_primitive_;
    };

    /// Shared reader instance
    inline constinit auto reader = Reader();
} // namespace memory
