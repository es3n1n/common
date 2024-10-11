#pragma once
#include <bit>
#include <cstddef>
#include <cstdint>
#include <expected>
#include <format>
#include <span>
#include <vector>

#include "es3n1n/common/traits.hpp"
#include "reader.hpp"

#if !defined(COMMON_PAGE_SIZE)
    #define COMMON_PAGE_SIZE 0x1000
#endif

namespace memory {
    /// \brief Represents a memory address with various operations and utilities
    class address {
    public:
        /// \brief Default constructor
        constexpr address() = default;

        // NOLINTBEGIN(google-explicit-constructor, hicpp-explicit-conversions)
        /// \brief Construct from nullptr
        constexpr /* implicit */ address(std::nullptr_t) noexcept { }

        /// \brief Construct from uintptr_t
        /// \param addr The address as a uintptr_t
        constexpr /* implicit */ address(uintptr_t addr) noexcept: address_(addr) { }

        /// \brief Construct from void pointer
        /// \param addr The address as a void pointer
        /* implicit */ address(const void* addr) noexcept: address_(reinterpret_cast<uintptr_t>(addr)) { }

        /// \brief Construct from span of bytes
        /// \param data The span of bytes
        /* implitic */ address(const std::span<const std::uint8_t>& data) noexcept: address_(reinterpret_cast<uintptr_t>(data.data())) { }
        // NOLINTEND(google-explicit-constructor, hicpp-explicit-conversions)

        /// Default copy and move operations
        address(const address&) = default;
        address(address&&) = default;
        address& operator=(const address&) = default;
        address& operator=(address&&) = default;
        ~address() = default;

        /// \brief Create a new address with an offset from this one
        /// \param offset The offset to add (default is 0)
        /// \return A new address object with the applied offset
        [[nodiscard]] constexpr address offset(std::ptrdiff_t offset = 0) const noexcept {
            return address_ == 0U ? *this : address{address_ + offset};
        }

        /// \brief Write data to the address
        /// \param buffer Pointer to the data to write
        /// \param size Size of the data to write
        /// \return Expected containing this address on success, or an error code on failure
        std::expected<address, e_error_code> write(const void* buffer, std::size_t size) {
            return memory::reader.write(address_, buffer, size).transform([this](auto) { return *this; });
        }

        /// \brief Write a trivially copyable value to the address
        /// \tparam Ty Type of the value to write (must be trivially copyable)
        /// \param value The value to write
        /// \return Expected containing this address on success, or an error code on failure
        template <traits::trivially_copyable Ty>
        std::expected<address, e_error_code> write(Ty value) {
            return memory::reader.write(&value, address_).transform([this](auto) { return *this; });
        }

        /// \brief Read a trivially copyable value from the address
        /// \tparam Ty Type of the value to read (must be trivially copyable)
        /// \return Expected containing the read value on success, or an error code on failure
        template <traits::trivially_copyable Ty>
        [[nodiscard]] std::expected<Ty, e_error_code> read() const {
            return memory::reader.read<Ty>(address_);
        }

        /// \brief Read a trivially copyable value from the address into a provided destination
        /// \tparam Ty Type of the value to read (must be trivially copyable)
        /// \param dst Pointer to the destination where the read value will be stored
        /// \return Expected containing the destination pointer on success, or an error code on failure
        template <traits::trivially_copyable Ty>
        std::expected<Ty*, e_error_code> read(Ty* dst) const {
            return memory::reader.read(dst, address_).transform([dst](auto) { return dst; });
        }

        /// \brief Read a vector of bytes from the address
        /// \param size Number of bytes to read
        /// \return Expected containing the read vector on success, or an error code on failure
        [[nodiscard]] std::expected<std::vector<std::uint8_t>, e_error_code> read_vector(std::size_t size) const {
            std::vector<std::uint8_t> result(size);
            return memory::reader.read(result.data(), address_, size).transform([&result](auto) { return result; });
        }

        /// \brief Dereference the address to read a value
        /// \tparam Ty Type of the value to read (default is address)
        /// \return Expected containing the dereferenced value on success, or an error code on failure
        template <traits::trivially_copyable Ty = address>
        [[nodiscard]] std::expected<Ty, e_error_code> deref() const {
            return memory::reader.read<Ty>(inner());
        }

        /// \brief Get a value by dereferencing the address multiple times
        /// \tparam Ty Type of the value to read (default is address)
        /// \param count Number of times to dereference (default is 1)
        /// \return Expected containing the final dereferenced value on success, or an error code on failure
        template <traits::trivially_copyable Ty = address>
        [[nodiscard]] std::expected<Ty, e_error_code> get(std::size_t count = 1) const noexcept {
            if (!address_ || count == 0) {
                return std::unexpected(e_error_code::INVALID_ADDRESS);
            }

            address tmp = *this;
            for (std::size_t i = 1; i < count; ++i) {
                auto deref_value = tmp.deref();
                if (!deref_value) {
                    return std::unexpected(e_error_code::NOT_ENOUGH_BYTES);
                }
                tmp = *deref_value;
            }

            return tmp.deref<Ty>();
        }

        /// \brief Get a pointer to the address with an optional offset
        /// \tparam Ty Type of the pointer (default is address)
        /// \param offset Offset to add to the address (default is 0)
        /// \return Pointer of type Ty* to the address (plus offset)
        template <traits::trivially_copyable Ty = address>
        [[nodiscard]] constexpr Ty* ptr(std::ptrdiff_t offset = 0) const noexcept {
            return this->offset(offset).as<std::add_pointer_t<Ty>>();
        }

        /// \brief Get a pointer to the address, then increment the address
        /// \tparam Ty Type of the pointer (default is address)
        /// \param offset Offset to add to the address before getting the pointer (default is 0)
        /// \return Pointer of type Ty* to the original address (plus offset)
        template <traits::trivially_copyable Ty = address>
        [[nodiscard]] constexpr Ty* self_inc_ptr(std::ptrdiff_t offset = 0) noexcept {
            Ty* result = ptr<Ty>(offset);
            *this = address{result}.offset(sizeof(Ty));
            return result;
        }

        /// \brief Write data to the address, then increment the address
        /// \tparam Ty Type of the data to write (must be trivially copyable)
        /// \param data The data to write
        /// \param offset Offset to add to the address before writing (default is 0)
        /// \return Expected containing this address on success, or an error code on failure
        template <traits::trivially_copyable Ty>
        std::expected<address, e_error_code> self_write_inc(const Ty& data, std::ptrdiff_t offset = 0) noexcept {
            auto result = this->offset(offset).write(data);
            *this = this->offset(offset + sizeof(Ty));
            return result;
        }

        /// \brief Align the address down to a multiple of the given factor
        /// \param factor The alignment factor
        /// \return A new address aligned down to the given factor
        [[nodiscard]] constexpr address align_down(std::size_t factor) const noexcept {
            return {address_ & ~(factor - 1U)};
        }

        /// \brief Align the address up to a multiple of the given factor
        /// \param factor The alignment factor
        /// \return A new address aligned up to the given factor
        [[nodiscard]] constexpr address align_up(std::size_t factor) const noexcept {
            return address{address_ + factor - 1U}.align_down(factor);
        }

        /// \brief Align the address to a page boundary
        [[nodiscard]] address page_align_up() const noexcept {
            return align_up(COMMON_PAGE_SIZE);
        }

        /// \brief Align the address down to a page boundary
        [[nodiscard]] address page_align_down() const noexcept {
            return align_down(COMMON_PAGE_SIZE);
        }

        /// \brief Cast the address to another type
        /// \tparam DstT The destination type
        /// \return The address cast to the specified type
        template <typename DstT>
        [[nodiscard]] constexpr DstT cast() const noexcept {
            if constexpr (std::is_same_v<std::remove_cv_t<DstT>, decltype(address_)>) {
                return address_;
            } else if constexpr (std::is_integral_v<DstT>) {
                return static_cast<DstT>(address_);
            } else {
                return reinterpret_cast<DstT>(address_);
            }
        }

        /// \brief Cast the address to another type (alias for cast)
        /// \tparam Ty The destination type (must be trivially copyable)
        /// \return The address cast to the specified type
        template <traits::trivially_copyable Ty>
        [[nodiscard]] constexpr Ty as() const noexcept {
            return cast<Ty>();
        }

        /// \brief Read an integral value in little-endian format
        /// \tparam Ty The integral type to read
        /// \return Expected containing the read value on success, or an error code on failure
        template <std::integral Ty>
        [[nodiscard]] std::expected<Ty, e_error_code> read_le() const {
            auto result = read<Ty>();
            if (!result) {
                return result;
            }
            if constexpr (std::endian::native == std::endian::big) {
                return std::byteswap(*result);
            }
            return result;
        }

        /// \brief Read an integral value in big-endian format
        /// \tparam Ty The integral type to read
        /// \return Expected containing the read value on success, or an error code on failure
        template <std::integral Ty>
        [[nodiscard]] std::expected<Ty, e_error_code> read_be() const {
            auto result = read<Ty>();
            if (!result) {
                return result;
            }
            if constexpr (std::endian::native == std::endian::little) {
                return std::byteswap(*result);
            }
            return result;
        }

        /// \brief Convert the address to a uintptr_t
        [[nodiscard]] constexpr explicit operator std::uintptr_t() const noexcept {
            return address_;
        }

        /// \brief Get the underlying uintptr_t value
        [[nodiscard]] constexpr std::uintptr_t inner() const noexcept {
            return address_;
        }

        /// \brief Check if the address is non-zero
        [[nodiscard]] constexpr explicit operator bool() const noexcept {
            return static_cast<bool>(address_);
        }

        /// Comparison operators
        /// \note: @annihilatorq: intentionally left with no return type, see #11
        constexpr auto operator<=>(const address&) const = default;

        /// Arithmetic operators
        constexpr address& operator+=(const address& rhs) noexcept {
            address_ += rhs.address_;
            return *this;
        }

        constexpr address& operator-=(const address& rhs) noexcept {
            address_ -= rhs.address_;
            return *this;
        }

        [[nodiscard]] constexpr address operator+(const address& rhs) const noexcept {
            return {address_ + rhs.address_};
        }

        [[nodiscard]] constexpr address operator-(const address& rhs) const noexcept {
            return {address_ - rhs.address_};
        }

        /// Bitwise operators
        [[nodiscard]] constexpr address operator&(const address& other) const noexcept {
            return {address_ & other.address_};
        }

        [[nodiscard]] constexpr address operator|(const address& other) const noexcept {
            return {address_ | other.address_};
        }

        [[nodiscard]] constexpr address operator^(const address& other) const noexcept {
            return {address_ ^ other.address_};
        }

        [[nodiscard]] constexpr address operator<<(std::size_t shift) const noexcept {
            return {address_ << shift};
        }

        [[nodiscard]] constexpr address operator>>(std::size_t shift) const noexcept {
            return {address_ >> shift};
        }

        /// \brief Convert the address to a string representation
        /// \return A string representation of the address in hexadecimal format
        [[nodiscard]] std::string to_string() const {
            return std::format("{:#x}", address_);
        }

        /// \brief Check if the address is aligned to a given boundary
        /// \param alignment The alignment boundary to check
        /// \return True if the address is aligned, false otherwise
        [[nodiscard]] bool is_aligned(std::size_t alignment) const noexcept {
            return (address_ % alignment) == 0;
        }

        /// \brief Get the relative offset from a base address
        /// \param base The base address
        [[nodiscard]] address relative_to(const address& base) const noexcept {
            return address{address_ - base.address_};
        }

        /// \brief Check if the address is within a given range
        /// \param start The start of the range
        /// \param end The end of the range
        /// \return True if the address is within the range, false otherwise
        [[nodiscard]] bool is_in_range(const address& start, const address& end) const noexcept {
            return *this >= start && *this < end;
        }

        /// \brief Calculate the distance to another address
        /// \param other The other address
        /// \return The distance between this address and the other address
        [[nodiscard]] std::ptrdiff_t distance_to(const address& other) const noexcept {
            return static_cast<std::ptrdiff_t>(other.address_ - address_);
        }

    private:
        std::uintptr_t address_ = 0; ///< The underlying address value
    };

} // namespace memory

/// Custom formatter for std::format to easily format addresses in logger
template <>
struct std::formatter<memory::address> : std::formatter<std::uintptr_t> {
    template <class FormatContextTy>
    constexpr auto format(const memory::address& instance, FormatContextTy& ctx) const {
        return std::formatter<std::uintptr_t>::format(instance.inner(), ctx);
    }
};

/// Custom hash implementation for std::hash to use this type in containers
template <>
struct std::hash<memory::address> {
    size_t operator()(const memory::address& instance) const noexcept {
        return std::hash<std::uintptr_t>()(instance.inner());
    }
};
