#include <es3n1n/common/platform.hpp>
#include <gtest/gtest.h>

TEST(platform, basics) {
    EXPECT_EQ(platform::bitness, sizeof(void*) * CHAR_BIT);
}
