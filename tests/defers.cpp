#include <es3n1n/common/defer.hpp>
#include <gtest/gtest.h>

TEST(defers, basics) {
    int counter = 0;

    auto nested = [&]() -> void {
        EXPECT_EQ(counter, 0);
        defer {
            counter += 1;
        };
        EXPECT_EQ(counter, 0);
    };

    EXPECT_EQ(counter, 0);
    nested();
    EXPECT_EQ(counter, 1);
}
