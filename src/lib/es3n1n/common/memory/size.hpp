#pragma once
#include "es3n1n/common/strong_integral.hpp"
#include <climits>

namespace memory {
    using Bits = types::StrongIntegral<std::size_t, struct BitsTag>;
    using Bytes = types::StrongIntegral<std::size_t, struct BytesTag>;

    inline constexpr Bits to_bits(const Bytes bytes) noexcept {
        return Bits(bytes.value() * CHAR_BIT);
    }

    inline constexpr Bytes to_bytes(const Bits bits) noexcept {
        return Bytes(bits.value() / CHAR_BIT);
    }

    template <typename Ty>
    inline constexpr Bytes size_of() noexcept {
        return Bytes(sizeof(Ty));
    }

    using Word = std::uintptr_t;
    constexpr static auto kWordInBits = to_bits(memory::size_of<Word>());
    constexpr static auto kHalfWordInBits = to_bits(memory::size_of<Word>()) / Bits(2);
} // namespace memory

inline constexpr memory::Bytes operator""_bytes(std::size_t val) noexcept {
    return memory::Bytes(val);
}

inline constexpr memory::Bits operator""_bits(std::size_t val) noexcept {
    return memory::Bits(val);
}
