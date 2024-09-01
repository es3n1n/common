#include "es3n1n/common/traits.hpp"
#include <string>

static_assert(!traits::always_false_v<>);
static_assert(!traits::always_false_v<int>);

static_assert(traits::is_any_of_v<int, char, short, int, bool>);
static_assert(!traits::is_any_of_v<int, char, short, bool>);

static_assert(traits::trivially_copyable<int>);
static_assert(!traits::trivially_copyable<std::string>);

static_assert(traits::number<int>);
static_assert(!traits::number<void>);

static_assert(traits::float_number<float>);
static_assert(!traits::float_number<int>);
