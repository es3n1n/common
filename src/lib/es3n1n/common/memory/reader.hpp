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
        UNKNOWN_ERROR = 0, ///< An unknown error occurred
        INVALID_PARAMETERS, ///< Invalid parameters (sizes, most likely) were provided
        INVALID_ADDRESS, ///< The provided address is invalid
        NOT_ENOUGH_BYTES, ///< Not enough bytes available for the operation
    };

    /// \brief Function pointer type for read primitive operations
    /// \param buffer Pointer to the output buffer
    /// \param address Source address to read from
    /// \param size Number of bytes to read
    /// \return Expected number of bytes read or an error code
    using ReadPrimitive = std::expected<std::size_t, ErrorCode> (*)(void* buffer, std::uintptr_t address, std::size_t size);

    /// \brief Function pointer type for write primitive operations
    /// \param address Destination address to write to
    /// \param buffer Pointer to the data to write
    /// \param size Number of bytes to write
    /// \return Expected number of bytes written or an error code
    using WritePrimitive = std::expected<std::size_t, ErrorCode> (*)(std::uintptr_t address, const void* buffer, std::size_t size);

    namespace detail {
        /// \brief Sanitize input parameters for memory operations
        /// \param buffer Pointer to the buffer
        /// \param address Memory address
        /// \param size Number of bytes
        /// \return Optional error code if parameters are invalid, nullopt otherwise
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

    /// \brief Default read primitive implementation
    /// \param buffer Pointer to the output buffer
    /// \param address Source address to read from
    /// \param size Number of bytes to read
    /// \return Expected number of bytes read or an error code
    inline std::expected<std::size_t, ErrorCode> default_read(void* buffer, const std::uintptr_t address, const std::size_t size) {
        if (auto err = detail::sanitize_parameters(buffer, address, size); err.has_value()) {
            return std::unexpected(err.value());
        }

        std::memcpy(buffer, reinterpret_cast<const void*>(address), size);
        return size;
    }

    /// \brief Default write primitive implementation
    /// \param address Destination address to write to
    /// \param buffer Pointer to the data to write
    /// \param size Number of bytes to write
    /// \return Expected number of bytes written or an error code
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
        /// \brief Default constructor
        constexpr Reader(): read_primitive_(default_read), write_primitive_(default_write) { }

        /// \brief Set the read primitive function
        /// \param read_func Function to use for read operations
        void read_primitive(ReadPrimitive read_func) {
            read_primitive_ = read_func;
        }

        /// \brief Set the write primitive function
        /// \param write_func Function to use for write operations
        void write_primitive(WritePrimitive write_func) {
            write_primitive_ = write_func;
        }

        /// \brief Read memory
        /// \param buffer Pointer to the output buffer
        /// \param address Source address to read from
        /// \param size Number of bytes to read
        /// \return Expected number of bytes read or an error code
        std::expected<std::size_t, ErrorCode> read(void* buffer, const std::uintptr_t address, const std::size_t size) const {
            return read_primitive_(buffer, address, size);
        }

        /// \brief Read a trivially copyable type from memory
        /// \tparam Ty Type to read (must be trivially copyable)
        /// \param dst Pointer to the destination object
        /// \param src Source address to read from
        /// \return Expected number of bytes read or an error code
        template <traits::TriviallyCopyable Ty>
        std::expected<std::size_t, ErrorCode> read(Ty* dst, const std::uintptr_t src) const {
            return read(dst, src, sizeof(Ty));
        }

        /// \brief Read and return a trivially copyable type from memory
        /// \tparam Ty Type to read (must be trivially copyable)
        /// \param src Source address to read from
        /// \return Expected object of type Ty or an error code
        template <traits::TriviallyCopyable Ty>
        std::expected<Ty, ErrorCode> read(const std::uintptr_t src) const {
            Ty obj = {};
            if (const auto res = read(&obj, src); !res.has_value()) {
                return std::unexpected(res.error());
            }
            return obj;
        }

        /// \brief Write memory
        /// \param address Destination address to write to
        /// \param buffer Pointer to the data to write
        /// \param size Number of bytes to write
        /// \return Expected number of bytes written or an error code
        std::expected<std::size_t, ErrorCode> write(const std::uintptr_t address, const void* buffer, const std::size_t size) const {
            return write_primitive_(address, buffer, size);
        }

        /// \brief Write a trivially copyable type to memory
        /// \tparam Ty Type to write (must be trivially copyable)
        /// \param src Pointer to the source object
        /// \param dst Destination address to write to
        /// \return Expected number of bytes written or an error code
        template <traits::TriviallyCopyable Ty>
        std::expected<std::size_t, ErrorCode> write(const Ty* src, const std::uintptr_t dst) const {
            return write(dst, src, sizeof(Ty));
        }

    private:
        ReadPrimitive read_primitive_;
        WritePrimitive write_primitive_;
    };

    inline constinit auto reader = Reader(); ///< Global memory reader instance
} // namespace memory
