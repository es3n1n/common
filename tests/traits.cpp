#include "es3n1n/common/traits.hpp"
#include <string>

static_assert(!traits::always_false_v<>);
static_assert(!traits::always_false_v<int>);

static_assert(traits::is_any_of_v<int, char, int16_t, int, bool>);
static_assert(!traits::is_any_of_v<int, char, int16_t, bool>);

static_assert(traits::TriviallyCopyable<int>);
static_assert(!traits::TriviallyCopyable<std::string>);

static_assert(traits::Number<int>);
static_assert(!traits::Number<void>);

static_assert(traits::FloatNumber<float>);
static_assert(!traits::FloatNumber<int>);
