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

template <int> concept always_false_i_v = false;

template <size_t I, char C>
static constexpr bool ensure() {
    if constexpr (I == 0) {
        static_assert(C == 'H');
    } else if constexpr (I == 1) {
        static_assert(C == 'e');
    } else if constexpr (I == 2) {
        static_assert(C == 'l');
    } else if constexpr (I == 3) {
        static_assert(C == 'l');
    } else if constexpr (I == 4) {
        static_assert(C == 'o');
    } else {
        static_assert(always_false_i_v<I> && always_false_i_v<C>);
    }
    return true;
}

template <class Lambda, size_t... I>
constexpr bool proceed(Lambda lambda [[maybe_unused]], std::index_sequence<I...>) {
    return (ensure<I, lambda()[I]>() && ...);
}

template <types::ct_string_t str>
consteval auto operator""_ct() noexcept {
    constexpr auto s = types::type_string_t<str>{};
    static_assert(str.size() == s.size());
    static_assert(str.size() == 5);
    return proceed([&]() constexpr { return s.data(); }, std::make_index_sequence<s.size()>{});
}

static_assert("Hello"_ct);
