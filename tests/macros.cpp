#include <cstdint>
#include <es3n1n/common/macros.hpp>

struct Aligned1 {
    std::uint8_t a;
    std::uint16_t b;
    std::uint16_t c;
};

COMMON_PACKED(struct Unaligned1 {
    std::uint8_t a;
    std::uint16_t b;
    std::uint16_t c;
});

struct Aligned2 {
    std::uint8_t a;
    std::uint16_t b;
    std::uint16_t c;
};

COMMON_PACKED_START;
struct Unaligned2 {
    std::uint8_t a;
    std::uint16_t b;
    std::uint16_t c;
};
COMMON_PACKED_END;

struct Aligned3 {
    std::uint8_t a;
    std::uint16_t b;
    std::uint16_t c;
};

static_assert(sizeof(Aligned1) == 6);
static_assert(sizeof(Aligned2) == 6);
static_assert(sizeof(Aligned3) == 6);
static_assert(sizeof(Unaligned1) == 5);
static_assert(sizeof(Unaligned2) == 5);
