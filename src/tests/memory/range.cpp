#include "es3n1n/common/memory/range.hpp"

static_assert(memory::Range{.start = 2, .end = 4}.size() == 2);
