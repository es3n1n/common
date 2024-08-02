#include <es3n1n/common/memory/address.hpp>
#include <gtest/gtest.h>

TEST(address, basics) {
    /// We aren't modifying zeroes
    EXPECT_EQ(memory::address(0ULL).offset(1).inner(), 0);

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
