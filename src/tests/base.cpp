#include "es3n1n/common/base.hpp"
#include <type_traits>

class TestNonCopyable : public base::NonCopyable {
public:
    TestNonCopyable() = default;
};

static_assert(!std::is_copy_constructible_v<TestNonCopyable>);
static_assert(!std::is_copy_assignable_v<TestNonCopyable>);

class TestCopyable {
public:
    TestCopyable() = default;
};

static_assert(std::is_copy_constructible_v<TestCopyable>);
static_assert(std::is_copy_assignable_v<TestCopyable>);

struct non_copyable_t : public base::NonCopyable {
public:
    non_copyable_t() = default;
};

static_assert(!std::is_copy_constructible_v<non_copyable_t>);
static_assert(!std::is_copy_assignable_v<non_copyable_t>);

struct copyable_t {
public:
    copyable_t() = default;
};

static_assert(std::is_copy_constructible_v<copyable_t>);
static_assert(std::is_copy_assignable_v<copyable_t>);
