#pragma once
#include "numeric.hpp"
#include <type_traits>
#include <variant>

namespace traits {
    /// \brief Concept for static assertions that always evaluate to false
    /// \tparam T A template parameter (defaulted to std::monostate)
    /// \details This concept is useful for creating compile-time errors in template specializations
    template <typename T = std::monostate> concept always_false_v = false;

    /// \brief Type trait to check if a type is any of the specified types
    /// \tparam Ty The type to check
    /// \tparam Types Pack of types to check against
    /// \returns true if Ty is any of the Types, false otherwise
    template <class Ty, class... Types>
    inline constexpr bool is_any_of_v = std::disjunction_v<std::is_same<Ty, Types>...>;

    /// \brief Concept to check if a type is trivially copyable
    /// \tparam Ty The type to check
    template <typename Ty> concept TriviallyCopyable = std::is_trivially_copyable_v<Ty>;

    /// \brief Concept to check if a type is a number (integral or floating-point)
    /// \tparam Ty The type to check
    template <typename Ty> concept Number = std::integral<Ty> || std::floating_point<Ty>;

    /// \brief Concept to check if a type is a floating-point number or a specific floating-point type
    /// \tparam Ty The type to check
    /// \details This concept checks for built-in floating-point types and a custom flt_range_t type
    template <typename Ty> concept FloatNumber = is_any_of_v<std::remove_cv_t<Ty>, float, double, long double, numeric::flt_range_t>;
} // namespace traits
