#pragma once
#include "numeric.hpp"
#include <type_traits>
#include <variant>

namespace traits {
    template <typename T = std::monostate> concept always_false_v = false;
    template <class Ty, class... Types>
    constexpr bool is_any_of_v = std::disjunction_v<std::is_same<Ty, Types>...>;

    template <typename Ty> concept TriviallyCopyable = std::is_trivially_copyable_v<Ty>;
    template <typename Ty> concept Number = std::integral<Ty> || std::floating_point<Ty>;
    template <typename Ty> concept FloatNumber = is_any_of_v<std::remove_cv_t<Ty>, float, double, long double, numeric::flt_range_t>;
} // namespace traits
