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

template <std::size_t I, char C> concept always_false_v = false;

template <std::size_t I, char C>
constexpr bool ensure() {
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
        static_assert(always_false_v<I, C>);
    }
    return true;
}

template <typename Ty, std::size_t... Is>
constexpr bool check_impl(std::index_sequence<Is...>) {
    return (ensure<Is, Ty::data()[Is]>() && ...);
}

template <types::ct_string_t Str>
consteval auto operator""_ct() {
    using s = types::type_string_t<Str>;
    static_assert(s::size() == 5);
    return check_impl<s>(std::make_index_sequence<s::size()>{});
}

static_assert("Hello"_ct);
