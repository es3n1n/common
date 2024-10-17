#include "es3n1n/common/hashes/crc.hpp"
#include <gtest/gtest.h>

/// Ensure compile-time hashing works
static_assert("hello"_crcb_32 == 0x3610a686);

TEST(crc, crc32b) {
    EXPECT_EQ("hello"_crcb_32, 0x3610a686);
    EXPECT_EQ(hashes::Crcb_32::hash("hello"), 0x3610a686);
}
