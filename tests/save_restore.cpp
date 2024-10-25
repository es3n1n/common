#include <es3n1n/common/save_restore.hpp>
#include <gtest/gtest.h>

TEST(save_restore, basics) {
    int x = 5;
    {
        SaveRestore _(x, 10);
        EXPECT_EQ(x, 10);
    }
    EXPECT_EQ(x, 5);
}
