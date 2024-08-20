#pragma once
#include <type_traits>
#include <variant>

namespace traits {
    /// Used for static assertions
    template <typename = std::monostate> concept always_false_v = false;

    template <class Ty, class... Types>
    inline constexpr bool is_any_of_v = std::disjunction_v<std::is_same<Ty, Types>...>;

    template <typename Ty> concept trivially_copyable = std::is_trivially_copyable_v<Ty>;
    template <typename Ty> concept default_constructible = std::is_default_constructible_v<Ty>;
}