#include <es3n1n/common/defer.hpp>
#include <es3n1n/common/random.hpp>
#include <gtest/gtest.h>

TEST(random, basics) {
    /// Shut up
    logger::enabled = false;
    defer {
        logger::enabled = true;
    };

    rnd::detail::seed(0x1234);

    EXPECT_EQ(rnd::number<uint8_t>(), 0x51);
    EXPECT_EQ(rnd::number<uint16_t>(), 0x9660);
    EXPECT_EQ(rnd::number<uint32_t>(), 0x138C3ADF);
    EXPECT_EQ(rnd::number<uint64_t>(), 0x7E70D1B4379F0328);

    auto bytes = rnd::bytes(5);
    EXPECT_EQ(bytes.size(), 5);
    EXPECT_EQ(bytes[0], 0xDB);
    EXPECT_EQ(bytes[1], 0xB8);
    EXPECT_EQ(bytes[2], 0x15);
    EXPECT_EQ(bytes[3], 0x2E);
    EXPECT_EQ(bytes[4], 0xBD);

    EXPECT_TRUE(rnd::chance(100));

    EXPECT_EQ(rnd::or_(1, 2, 3), 2);
}
