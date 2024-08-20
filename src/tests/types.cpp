#include <es3n1n/common/types.hpp>
#include <gtest/gtest.h>

class TestSingleton : public types::Singleton<TestSingleton> {
public:
    [[nodiscard]] int five() const noexcept { // NOLINT
        return 5;
    }
};

TEST(types, basics) {
    EXPECT_EQ(TestSingleton::get().five(), 5);

    auto v = types::to_array(5, 6, 7);
    EXPECT_EQ(v.size(), 3);
    EXPECT_EQ(v[0], 5);
    EXPECT_EQ(v[1], 6);
    EXPECT_EQ(v[2], 7);
}
