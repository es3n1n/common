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

    EXPECT_EQ(rnd::number<uint8_t>(), 0xCD);
    EXPECT_EQ(rnd::number<uint16_t>(), 0x9660);
    EXPECT_EQ(rnd::number<uint32_t>(), 0x138C3ADF);
    EXPECT_EQ(rnd::number<uint64_t>(), 0x7E70D1B4379F0328);

    auto bytes = rnd::bytes(5);
    EXPECT_EQ(bytes.size(), 5);
    EXPECT_EQ(bytes[0], 0xA);
    EXPECT_EQ(bytes[1], 0xD4);
    EXPECT_EQ(bytes[2], 0xD5);
    EXPECT_EQ(bytes[3], 0xDA);
    EXPECT_EQ(bytes[4], 0x7C);

    EXPECT_TRUE(rnd::chance(100));

    EXPECT_EQ(rnd::or_(1, 2, 3), 2);

    EXPECT_EQ(rnd::number<uint32_t>(50, 100), 58);
    EXPECT_EQ(rnd::number<uint32_t>(50, 100), 53);
    EXPECT_EQ(rnd::number<uint32_t>(50, 100), 88);
    EXPECT_EQ(rnd::number<uint32_t>(50, 100), 73);
    EXPECT_EQ(rnd::number<uint32_t>(50, 100), 58);
    EXPECT_EQ(rnd::number<uint32_t>(50, 100), 82);
    EXPECT_EQ(rnd::number<uint32_t>(50, 100), 96);
    EXPECT_EQ(rnd::number<uint32_t>(50, 100), 70);
    EXPECT_EQ(rnd::number<uint32_t>(50, 100), 93);
}
