#include "es3n1n/common/numeric.hpp"

static_assert(std::numeric_limits<numeric::flt_range_t>::min() == 0.F);
static_assert(std::numeric_limits<numeric::flt_range_t>::max() == 1.F);
