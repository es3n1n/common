#include "es3n1n/common/platform.hpp"
#include <array>
#include <cstdint>
#include <es3n1n/common/memory/address.hpp>
#include <gtest/gtest.h>

TEST(address, basics) {
    /// We aren't modifying zeroes
    EXPECT_EQ(memory::address(nullptr).offset(1).inner(), 0);
    EXPECT_EQ(memory::address(1ULL).offset(1).inner(), 2);
}

/// We are testing memory reader here too

TEST(address, readwrite) {
    uint8_t buffer[sizeof(void*)] = {};

    memory::address addr = buffer;

    EXPECT_EQ(addr.read<std::uint32_t>().value(), 0x00);
    EXPECT_TRUE(addr.write<std::uint32_t>(0x11223344).has_value());
    EXPECT_EQ(addr.read<std::uint32_t>().value(), 0x11223344);

    uint32_t tmp;
    EXPECT_TRUE(addr.read(&tmp).has_value());
    EXPECT_EQ(tmp, 0x11223344);

    auto vec = addr.read_vector(4).value();
    EXPECT_EQ(vec[0], 0x44);
    EXPECT_EQ(vec[1], 0x33);
    EXPECT_EQ(vec[2], 0x22);
    EXPECT_EQ(vec[3], 0x11);

    EXPECT_TRUE(addr.write(buffer).has_value());
    EXPECT_EQ(addr.deref().value(), addr);
    EXPECT_EQ(addr.get(5).value(), addr);

    *addr.ptr<uint8_t>() = 0x00;
    EXPECT_EQ(buffer[0], 0x00);

    EXPECT_NE(addr.self_inc_ptr<uint8_t>(), nullptr);
    EXPECT_EQ(addr.inner(), reinterpret_cast<std::uintptr_t>(buffer + 1));

    addr = addr.offset(-1);
    EXPECT_TRUE(addr.self_write_inc<std::uint8_t>(5).has_value());
    EXPECT_EQ(addr.inner(), reinterpret_cast<std::uintptr_t>(buffer + 1));

    addr = addr.offset(-1);
    EXPECT_EQ(addr.read<std::uint8_t>().value(), 5);
}

TEST(address, aligns) {
    EXPECT_EQ(memory::address(0x1234).align_down(0x1000), 0x1000);
    EXPECT_EQ(memory::address(0x2234).align_down(0x1000), 0x2000);
    EXPECT_EQ(memory::address(0x2000).align_down(0x1000), 0x2000);

    EXPECT_EQ(memory::address(0x1234).align_up(0x1000), 0x2000);
    EXPECT_EQ(memory::address(0x2234).align_up(0x1000), 0x3000);
    EXPECT_EQ(memory::address(0x2000).align_up(0x1000), 0x2000);
}

TEST(address, operators) {
    memory::address addr1(0x1000);
    memory::address addr2(0x2000);
    memory::address addr3(0x1000);

    EXPECT_EQ((addr1 + addr2).inner(), 0x3000);
    EXPECT_EQ((addr2 - addr1).inner(), 0x1000);

    EXPECT_TRUE(addr1 < addr2);
    EXPECT_TRUE(addr2 > addr1);
    EXPECT_TRUE(addr1 <= addr3);
    EXPECT_TRUE(addr1 >= addr3);

    EXPECT_TRUE(addr1 == addr3);
    EXPECT_TRUE(addr1 != addr2);

    EXPECT_TRUE(static_cast<bool>(addr1));
    EXPECT_FALSE(static_cast<bool>(memory::address(nullptr)));
    EXPECT_EQ(static_cast<std::uintptr_t>(addr1), 0x1000);
}

TEST(address, casting) {
    memory::address addr(0x12345678);

    EXPECT_EQ(addr.cast<std::uint32_t>(), static_cast<std::uint32_t>(0x12345678));
    EXPECT_EQ(addr.cast<std::uint64_t>(), static_cast<std::uint64_t>(0x12345678));
    EXPECT_EQ(addr.cast<std::int32_t>(), static_cast<std::int32_t>(0x12345678));

    EXPECT_EQ(addr.as<std::uintptr_t>(), static_cast<std::uintptr_t>(0x12345678));

    int* ptr = addr.as<int*>();
    EXPECT_EQ(reinterpret_cast<std::uintptr_t>(ptr), static_cast<std::uintptr_t>(0x12345678));
}

TEST(address, constructors) {
    EXPECT_EQ(memory::address(nullptr).inner(), 0);
    EXPECT_EQ(memory::address(0x1234ULL).inner(), 0x1234ULL);

    int dummy = 0;
    EXPECT_EQ(memory::address(&dummy).inner(), reinterpret_cast<std::uintptr_t>(&dummy));

    std::array<uint8_t, 4> data = {1, 2, 3, 4};
    EXPECT_EQ(memory::address(std::span(data)).inner(), reinterpret_cast<std::uintptr_t>(data.data()));
}

TEST(address, assignment_operators) {
    memory::address addr1(0x1000);
    memory::address addr2(0x2000);

    addr1 += addr2;
    EXPECT_EQ(addr1.inner(), 0x3000ULL);

    addr1 -= addr2;
    EXPECT_EQ(addr1.inner(), 0x1000ULL);
}

TEST(address, error_handling) {
    memory::address addr(nullptr);

    auto read_result = addr.read<int>();
    EXPECT_FALSE(read_result.has_value());
    EXPECT_EQ(read_result.error(), memory::ErrorCode::INVALID_ADDRESS);

    int value = 42;
    auto write_result = addr.write(value);
    EXPECT_FALSE(write_result.has_value());
    EXPECT_EQ(write_result.error(), memory::ErrorCode::INVALID_ADDRESS);

    auto get_result = addr.get<int>(5);
    EXPECT_FALSE(get_result.has_value());
    EXPECT_EQ(get_result.error(), memory::ErrorCode::INVALID_ADDRESS);
}

TEST(address, formatting) {
    memory::address addr(0x1234ABCD);
    std::string formatted = std::format("{:x}", addr);
    EXPECT_EQ(formatted, "1234abcd");
}

TEST(address, hashing) {
    memory::address addr1(0x1234);
    memory::address addr2(0x1234);
    memory::address addr3(0x5678);

    std::hash<memory::address> hasher;
    EXPECT_EQ(hasher(addr1), hasher(addr2));
    EXPECT_NE(hasher(addr1), hasher(addr3));
}

TEST(address, edge_cases) {
    memory::address max_addr(std::numeric_limits<std::uintptr_t>::max());
    EXPECT_EQ(max_addr.inner(), std::numeric_limits<std::uintptr_t>::max());

    EXPECT_EQ(max_addr.offset(1).inner(), 0);
    EXPECT_EQ(max_addr.align_up(2).inner(), 0);
}

TEST(address, bounds_checking) {
    memory::address addr(0x1000);
    EXPECT_TRUE(addr.is_in_range(memory::address(0x500), memory::address(0x1500)));
    EXPECT_FALSE(addr.is_in_range(memory::address(0x1500), memory::address(0x2000)));
}

TEST(address, relative_addressing) {
    memory::address addr1(0x1000);
    memory::address addr2(0x1500);
    EXPECT_EQ(addr1.distance_to(addr2), 0x500);
    EXPECT_EQ(addr2.distance_to(addr1), -0x500);
}

TEST(address, endianness) {
    uint32_t value = 0x12345678;
    memory::address addr(&value);

    EXPECT_EQ(addr.read_le<uint32_t>().value(), std::endian::native == std::endian::little ? 0x12345678 : 0x78563412);
    EXPECT_EQ(addr.read_be<uint32_t>().value(), std::endian::native == std::endian::big ? 0x12345678 : 0x78563412);
}

TEST(address, bitwise_operations) {
    memory::address addr1(0xFF00);
    memory::address addr2(0x00FF);
    EXPECT_EQ((addr1 & addr2).inner(), 0x0000);
    EXPECT_EQ((addr1 | addr2).inner(), 0xFFFF);
    EXPECT_EQ((addr1 ^ addr2).inner(), 0xFFFF);

    memory::address addr(0x1234);
    EXPECT_EQ(addr << 4, memory::address(0x12340));
    EXPECT_EQ(addr >> 4, memory::address(0x123));
    EXPECT_EQ(addr << 8, memory::address(0x123400));
    EXPECT_EQ(addr >> 8, memory::address(0x12));
    EXPECT_EQ(addr << 0, addr);
    EXPECT_EQ(addr >> 0, addr);
    EXPECT_EQ(addr >> 16, memory::address(nullptr));

    memory::address max_addr(std::numeric_limits<std::uintptr_t>::max());
    EXPECT_EQ(max_addr << 1, memory::address(std::numeric_limits<std::uintptr_t>::max() << 1));

    if constexpr (platform::is_x64) {
        memory::address large_addr(0x1234567890ABCDEF);
        EXPECT_EQ(large_addr >> 32, memory::address(0x12345678));
    }
}

TEST(address, string_conversion) {
    memory::address addr(0x1234);
    EXPECT_EQ(addr.to_string(), "0x1234");
}

TEST(address, alignment) {
    EXPECT_TRUE(memory::address(0x1000).is_aligned(16));
    EXPECT_FALSE(memory::address(0x1001).is_aligned(16));
}

TEST(address, relative_to) {
    memory::address base(0x1000);
    memory::address addr(0x1500);
    memory::address lower(0x500);

    EXPECT_EQ(addr.relative_to(base), memory::address(0x500));
    EXPECT_EQ(lower.relative_to(base), memory::address(0x500 - 0x1000));
    EXPECT_EQ(base.relative_to(base), memory::address(nullptr));
}

TEST(address, page_aligns) {
    memory::address addr1(0x1234);
    EXPECT_EQ(addr1.page_align_down(), memory::address(0x1000));
    EXPECT_EQ(addr1.page_align_up(), memory::address(0x2000));

    memory::address addr2(0x2000);
    EXPECT_EQ(addr2.page_align_down(), memory::address(0x2000));
    EXPECT_EQ(addr2.page_align_up(), memory::address(0x2000));

    memory::address addr3(0x2001);
    EXPECT_EQ(addr3.page_align_down(), memory::address(0x2000));
    EXPECT_EQ(addr3.page_align_up(), memory::address(0x3000));
}

template <typename T>
inline std::uintptr_t GetAbsAddr(const std::uintptr_t addr, const std::ptrdiff_t disp_offset, const std::size_t insn_size) {

    const auto displacement = *reinterpret_cast<T*>(addr + disp_offset);

    return (addr + insn_size) + displacement;
}

// note: inf: emulate instruction layout in memory for unit tests
TEST(address, absolute_addressing_rel32) {
    std::array<std::uint8_t, 16> memory_buffer{};
    memory::Address insn_start = memory_buffer.data();

    constexpr std::ptrdiff_t disp_offset = 3;
    constexpr std::int16_t displacement = 0x50;
    constexpr std::size_t insn_size = disp_offset + sizeof(std::int32_t);

    *reinterpret_cast<std::int32_t*>(&memory_buffer[disp_offset]) = displacement;

    const std::uintptr_t expected_addr = GetAbsAddr<std::int32_t>(insn_start.inner(), disp_offset, insn_size);
    auto result = insn_start.rel32(disp_offset);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->inner(), expected_addr);
    EXPECT_EQ(result->inner(), insn_start.inner() + insn_size + displacement);
}

TEST(address, absolute_addressing_rel16) {
    std::array<std::uint8_t, 16> memory_buffer{};
    memory::Address insn_start = memory_buffer.data();

    constexpr std::ptrdiff_t disp_offset = 2;
    constexpr std::int16_t displacement = -0x20;
    constexpr std::size_t insn_size = disp_offset + sizeof(std::int16_t);

    *reinterpret_cast<std::int16_t*>(&memory_buffer[disp_offset]) = displacement;

    const std::uintptr_t expected_addr = GetAbsAddr<std::int16_t>(insn_start.inner(), disp_offset, insn_size);
    const auto result = insn_start.rel16(disp_offset);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->inner(), expected_addr);
    EXPECT_EQ(result->inner(), insn_start.inner() + insn_size + displacement);
}

TEST(address, absolute_addressing_rel8) {
    std::array<std::uint8_t, 16> memory_buffer{};
    memory::Address insn_start = memory_buffer.data();

    constexpr std::ptrdiff_t disp_offset = 1;
    constexpr std::int8_t displacement = 0x1A;
    constexpr std::size_t insn_size = disp_offset + sizeof(std::int8_t);

    *reinterpret_cast<std::int8_t*>(&memory_buffer[disp_offset]) = displacement;

    const std::uintptr_t expected_addr = GetAbsAddr<std::int8_t>(insn_start.inner(), disp_offset, insn_size);

    const auto result = insn_start.rel8(disp_offset);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->inner(), expected_addr);
    EXPECT_EQ(result->inner(), insn_start.inner() + insn_size + displacement);
}

TEST(address, absolute_addressing_error_handling) {
    memory::Address null_addr(nullptr);

    const auto result = null_addr.rel32();

    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), memory::ErrorCode::INVALID_ADDRESS);
}