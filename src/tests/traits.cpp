#include <string>
#include "es3n1n/common/traits.hpp"

static_assert(!traits::always_false_v<>);
static_assert(!traits::always_false_v<int>);

static_assert(traits::is_any_of_v<int, char, short, int, bool>);
static_assert(!traits::is_any_of_v<int, char, short, bool>);

static_assert(traits::trivially_copyable<int>);
static_assert(!traits::trivially_copyable<std::string>);
