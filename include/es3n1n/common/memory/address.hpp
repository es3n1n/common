#pragma once
#include <bit>
#include <cstddef>
#include <cstdint>
#include <expected>
#include <format>
#include <span>
#include <vector>

#include "es3n1n/common/numeric.hpp"
#include "es3n1n/common/options.hpp"
#include "es3n1n/common/traits.hpp"
#include "reader.hpp"

namespace memory {
    /// \brief Represents a memory address with various operations and utilities
    class Address {
    public:
        constexpr Address() = default;

        // NOLINTBEGIN(google-explicit-constructor, hicpp-explicit-conversions)
        constexpr /* implicit */ Address(std::nullptr_t) noexcept { }
        constexpr /* implicit */ Address(uintptr_t addr) noexcept: address_(addr) { }
        /* implicit */ Address(const void* addr) noexcept: address_(reinterpret_cast<uintptr_t>(addr)) { }
        /* implitic */ Address(const std::span<const std::uint8_t>& data) noexcept: address_(reinterpret_cast<uintptr_t>(data.data())) { }
        // NOLINTEND(google-explicit-constructor, hicpp-explicit-conversions)

        Address(const Address&) = default;
        Address(Address&&) = default;
        Address& operator=(const Address&) = default;
        Address& operator=(Address&&) = default;
        ~Address() = default;

        [[nodiscard]] constexpr Address offset(std::ptrdiff_t offset = 0) const noexcept {
            return address_ == 0U ? *this : Address{address_ + offset};
        }

        std::expected<Address, ErrorCode> write(const void* buffer, std::size_t size) {
            return memory::reader.write(address_, buffer, size).transform([this](auto) { return *this; });
        }

        template <traits::TriviallyCopyable Ty>
        std::expected<Address, ErrorCode> write(Ty value) {
            return memory::reader.write(&value, address_).transform([this](auto) { return *this; });
        }

        template <traits::TriviallyCopyable Ty>
        [[nodiscard]] std::expected<Ty, ErrorCode> read() const {
            return memory::reader.read<Ty>(address_);
        }

        template <traits::TriviallyCopyable Ty>
        std::expected<Ty*, ErrorCode> read(Ty* dst) const {
            return memory::reader.read(dst, address_).transform([dst](auto) { return dst; });
        }

        [[nodiscard]] std::expected<std::vector<std::uint8_t>, ErrorCode> read_vector(std::size_t size) const {
            std::vector<std::uint8_t> result(size);
            return memory::reader.read(result.data(), address_, size).transform([&result](auto) { return result; });
        }

        template <traits::TriviallyCopyable Ty = Address>
        [[nodiscard]] std::expected<Ty, ErrorCode> deref() const {
            return memory::reader.read<Ty>(inner());
        }

        template <traits::TriviallyCopyable Ty = Address>
        [[nodiscard]] std::expected<Ty, ErrorCode> get(std::size_t count = 1) const noexcept {
            if (!address_ || count == 0) {
                return std::unexpected(ErrorCode::INVALID_ADDRESS);
            }

            auto tmp = *this;
            for (std::size_t i = 1; i < count; ++i) {
                auto deref_value = tmp.deref();
                if (!deref_value) {
                    return std::unexpected(ErrorCode::NOT_ENOUGH_BYTES);
                }
                tmp = *deref_value;
            }

            return tmp.deref<Ty>();
        }

        template <traits::TriviallyCopyable Ty = Address>
        [[nodiscard]] constexpr Ty* ptr(std::ptrdiff_t offset = 0) const noexcept {
            return this->offset(offset).as<std::add_pointer_t<Ty>>();
        }

        template <traits::TriviallyCopyable Ty = Address>
        [[nodiscard]] constexpr Ty* self_inc_ptr(std::ptrdiff_t offset = 0) noexcept {
            Ty* result = ptr<Ty>(offset);
            *this = Address{result}.offset(sizeof(Ty));
            return result;
        }

        template <traits::TriviallyCopyable Ty>
        std::expected<Address, ErrorCode> self_write_inc(const Ty& data, std::ptrdiff_t offset = 0) noexcept {
            auto result = this->offset(offset).write(data);
            *this = this->offset(offset + sizeof(Ty));
            return result;
        }

        [[nodiscard]] constexpr Address align_down(std::size_t factor) const noexcept {
            return {address_ & ~(factor - 1U)};
        }

        [[nodiscard]] constexpr Address align_up(std::size_t factor) const noexcept {
            return Address{address_ + factor - 1U}.align_down(factor);
        }

        [[nodiscard]] Address page_align_up() const noexcept {
            return align_up(COMMON_PAGE_SIZE);
        }

        [[nodiscard]] Address page_align_down() const noexcept {
            return align_down(COMMON_PAGE_SIZE);
        }

        template <typename DstT>
        [[nodiscard]] constexpr DstT cast() const noexcept(std::is_nothrow_constructible_v<DstT>) {
            if constexpr (std::is_same_v<std::remove_cv_t<DstT>, decltype(address_)>) {
                return address_;
            } else if constexpr (std::is_integral_v<DstT>) {
                return static_cast<DstT>(address_);
            } else {
                return reinterpret_cast<DstT>(address_);
            }
        }

        template <traits::TriviallyCopyable Ty>
        [[nodiscard]] constexpr Ty as() const noexcept {
            return cast<Ty>();
        }

        template <std::integral Ty>
        [[nodiscard]] std::expected<Ty, ErrorCode> read_le() const {
            auto value = read<Ty>();
            if (!value) {
                return std::unexpected(value.error());
            }
            return numeric::to_le(*value);
        }

        template <std::integral Ty>
        [[nodiscard]] std::expected<Ty, ErrorCode> read_be() const {
            auto value = read<Ty>();
            if (!value) {
                return std::unexpected(value.error());
            }
            return numeric::to_be(*value);
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

        /// \note: @annihilatorq: intentionally left with no return type, see #11
        constexpr auto operator<=>(const Address&) const = default;

        constexpr Address& operator+=(const Address& rhs) noexcept {
            address_ += rhs.address_;
            return *this;
        }

        constexpr Address& operator-=(const Address& rhs) noexcept {
            address_ -= rhs.address_;
            return *this;
        }

        [[nodiscard]] constexpr Address operator+(const Address& rhs) const noexcept {
            return {address_ + rhs.address_};
        }

        [[nodiscard]] constexpr Address operator-(const Address& rhs) const noexcept {
            return {address_ - rhs.address_};
        }

        [[nodiscard]] constexpr Address operator&(const Address& other) const noexcept {
            return {address_ & other.address_};
        }

        [[nodiscard]] constexpr Address operator|(const Address& other) const noexcept {
            return {address_ | other.address_};
        }

        [[nodiscard]] constexpr Address operator^(const Address& other) const noexcept {
            return {address_ ^ other.address_};
        }

        [[nodiscard]] constexpr Address operator<<(std::size_t shift) const noexcept {
            return {address_ << shift};
        }

        [[nodiscard]] constexpr Address operator>>(std::size_t shift) const noexcept {
            return {address_ >> shift};
        }

        [[nodiscard]] std::string to_string() const {
            return std::format("{:#x}", address_);
        }

        [[nodiscard]] bool is_aligned(std::size_t alignment) const noexcept {
            return (address_ % alignment) == 0;
        }

        [[nodiscard]] Address relative_to(const Address& base) const noexcept {
            return Address{address_ - base.address_};
        }

        [[nodiscard]] bool is_in_range(const Address& start, const Address& end) const noexcept {
            return *this >= start && *this < end;
        }

        [[nodiscard]] std::ptrdiff_t distance_to(const Address& other) const noexcept {
            return static_cast<std::ptrdiff_t>(other.address_ - address_);
        }

    private:
        std::uintptr_t address_ = 0;
    };

    /// Backporting old version of address to Address
    using address = Address;
} // namespace memory

template <>
struct std::formatter<memory::Address> : std::formatter<std::uintptr_t> {
    template <class FormatContextTy>
    constexpr auto format(const memory::Address& instance, FormatContextTy& ctx) const {
        return std::formatter<std::uintptr_t>::format(instance.inner(), ctx);
    }
};

template <>
struct std::hash<memory::Address> {
    std::size_t operator()(const memory::Address& instance) const noexcept {
        return std::hash<std::uintptr_t>()(instance.inner());
    }
};
