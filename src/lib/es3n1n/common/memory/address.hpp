#pragma once
#include <cstddef>
#include <cstdint>
#include <expected>
#include <format>
#include <span>
#include <vector>

#include "es3n1n/common/traits.hpp"
#include "reader.hpp"

namespace memory {
    class address {
    public:
        constexpr address() = default;

        // NOLINTBEGIN(google-explicit-constructor, hicpp-explicit-conversions)
        constexpr /* implicit */ address(std::nullptr_t) noexcept { }
        constexpr /* implicit */ address(uintptr_t addr) noexcept: address_(addr) { }
        /* implicit */ address(const void* addr) noexcept: address_(reinterpret_cast<uintptr_t>(addr)) { }
        /* implitic */ address(const std::span<const std::uint8_t>& data) noexcept: address_(reinterpret_cast<uintptr_t>(data.data())) { }
        // NOLINTEND(google-explicit-constructor, hicpp-explicit-conversions)

        address(const address&) = default;
        address(address&&) = default;
        address& operator=(const address&) = default;
        address& operator=(address&&) = default;
        ~address() = default;

        [[nodiscard]] constexpr address offset(std::ptrdiff_t offset = 0) const noexcept {
            return address_ == 0U ? *this : address{address_ + offset};
        }

        std::expected<address, e_error_code> write(const void* buffer, std::size_t size) {
            return memory::reader.write(address_, buffer, size).transform([this](auto) { return *this; });
        }

        template <traits::trivially_copyable Ty>
        std::expected<address, e_error_code> write(Ty value) {
            return memory::reader.write(&value, address_).transform([this](auto) { return *this; });
        }

        template <traits::trivially_copyable Ty>
        [[nodiscard]] std::expected<Ty, e_error_code> read() const {
            return memory::reader.read<Ty>(address_);
        }

        template <traits::trivially_copyable Ty>
        std::expected<Ty*, e_error_code> read(Ty* dst) const {
            return memory::reader.read(dst, address_).transform([dst](auto) { return dst; });
        }

        [[nodiscard]] std::expected<std::vector<std::uint8_t>, e_error_code> read_vector(std::size_t size) const {
            std::vector<std::uint8_t> result(size);
            return memory::reader.read(result.data(), address_, size).transform([&result](auto) { return result; });
        }

        template <traits::trivially_copyable Ty = address>
        [[nodiscard]] std::expected<Ty, e_error_code> deref() const {
            return memory::reader.read<Ty>(inner());
        }

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

        template <traits::trivially_copyable Ty = address>
        [[nodiscard]] constexpr Ty* ptr(std::ptrdiff_t offset = 0) const noexcept {
            return this->offset(offset).as<std::add_pointer_t<Ty>>();
        }

        template <traits::trivially_copyable Ty = address>
        [[nodiscard]] constexpr Ty* self_inc_ptr(std::ptrdiff_t offset = 0) noexcept {
            Ty* result = ptr<Ty>(offset);
            *this = address{result}.offset(sizeof(Ty));
            return result;
        }

        template <traits::trivially_copyable Ty>
        std::expected<address, e_error_code> self_write_inc(const Ty& data, std::ptrdiff_t offset = 0) noexcept {
            auto result = this->offset(offset).write(data);
            *this = this->offset(offset + sizeof(Ty));
            return result;
        }

        [[nodiscard]] constexpr address align_down(std::size_t factor) const noexcept {
            return {address_ & ~(factor - 1U)};
        }

        [[nodiscard]] constexpr address align_up(std::size_t factor) const noexcept {
            return address{address_ + factor - 1U}.align_down(factor);
        }

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

        template <traits::trivially_copyable Ty>
        [[nodiscard]] constexpr Ty as() const noexcept {
            return cast<Ty>();
        }

        [[nodiscard]] bool is_in_range(const address& start, const address& end) const noexcept {
            return *this >= start && *this < end;
        }

        [[nodiscard]] std::ptrdiff_t distance_to(const address& other) const noexcept {
            return static_cast<std::ptrdiff_t>(other.address_ - address_);
        }

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

        [[nodiscard]] constexpr explicit operator std::uintptr_t() const noexcept {
            return address_;
        }

        [[nodiscard]] constexpr std::uintptr_t inner() const noexcept {
            return address_;
        }

        [[nodiscard]] constexpr explicit operator bool() const noexcept {
            return static_cast<bool>(address_);
        }

        // \note: @annihilatorq: intentionally left with no return type, see #11
        constexpr auto operator<=>(const address&) const = default;

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

        [[nodiscard]] constexpr address operator&(const address& other) const noexcept {
            return {address_ & other.address_};
        }

        [[nodiscard]] constexpr address operator|(const address& other) const noexcept {
            return {address_ | other.address_};
        }

        [[nodiscard]] constexpr address operator^(const address& other) const noexcept {
            return {address_ ^ other.address_};
        }

        [[nodiscard]] std::string to_string() const {
            return std::format("{:#x}", address_);
        }

        [[nodiscard]] bool is_aligned(std::size_t alignment) const noexcept {
            return (address_ % alignment) == 0;
        }

    private:
        std::uintptr_t address_ = 0;
    };
} // namespace memory

/// Creating custom formatters for the std::format function so that
/// we can easily format addresses in logger
///
template <>
struct std::formatter<memory::address> : std::formatter<std::uintptr_t> {
    template <class FormatContextTy>
    constexpr auto format(const memory::address& instance, FormatContextTy& ctx) const {
        return std::formatter<std::uintptr_t>::format(instance.inner(), ctx);
    }
};

/// Overriding custom implementation for std::hash in order to use
/// this type in containers
///
template <>
struct std::hash<memory::address> {
    size_t operator()(const memory::address& instance) const noexcept {
        return std::hash<std::uintptr_t>()(instance.inner());
    }
};
