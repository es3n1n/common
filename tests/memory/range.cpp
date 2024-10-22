#include "es3n1n/common/memory/range.hpp"

static_assert(memory::Range{.start = nullptr, .end = 10}.size() == 10);
static_assert(memory::Range{.start = 100, .end = 200}.size() == 100);
static_assert(memory::Range{.start = nullptr, .end = nullptr}.size() == 0);

static_assert(memory::Range{.start = nullptr, .end = nullptr}.is_empty());
static_assert(!memory::Range{.start = nullptr, .end = 1}.is_empty());

static_assert(memory::Range{.start = nullptr, .end = 10}.contains(5));
static_assert(memory::Range{.start = nullptr, .end = 10}.contains(nullptr));
static_assert(!memory::Range{.start = nullptr, .end = 10}.contains(10));
static_assert(!memory::Range{.start = nullptr, .end = 10}.contains(15));

static_assert(memory::Range{.start = nullptr, .end = 10}.overlaps(memory::Range{.start = 5, .end = 15}));
static_assert(memory::Range{.start = nullptr, .end = 10}.overlaps(memory::Range{.start = nullptr, .end = 5}));
static_assert(memory::Range{.start = nullptr, .end = 10}.overlaps(memory::Range{.start = nullptr, .end = 10}));
static_assert(!memory::Range{.start = nullptr, .end = 10}.overlaps(memory::Range{.start = 10, .end = 20}));
static_assert(!memory::Range{.start = nullptr, .end = 10}.overlaps(memory::Range{.start = 20, .end = 30}));

static_assert(memory::Range{.start = nullptr, .end = static_cast<memory::Address>(-1)}.size() == static_cast<std::size_t>(-1));

static_assert(std::is_standard_layout_v<memory::Range>);
static_assert(std::is_trivially_copyable_v<memory::Range>);
