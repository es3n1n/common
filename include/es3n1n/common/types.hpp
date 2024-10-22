#pragma once
#include <algorithm>
#include <array>
#include <type_traits>

#include "base.hpp"
#include "memory/address.hpp"
#include "strong_integral.hpp"
#include "traits.hpp"

namespace types {
    template <typename... Args>
    [[nodiscard]] constexpr auto to_array(Args&&... args) {
        return std::array<std::common_type_t<Args...>, sizeof...(Args)>{std::forward<Args>(args)...};
    }

    /// \brief A template class for creating singleton objects
    /// \tparam Ty The type of the singleton object
    template <typename Ty>
    class Singleton : public base::NonCopyable {
    public:
        [[nodiscard]] static Ty& get() {
            static Ty instance = {};
            return instance;
        }

    protected:
        Singleton() = default;
    };

    /// \brief A compile-time string class
    /// \tparam N The size of the string including the null terminator
    template <std::size_t N>
    struct CtString {
        constexpr /* implicit */ CtString(const char (&init)[N]) {
            std::copy_n(init, N, data.begin());
        }

        [[nodiscard]] constexpr std::size_t size() const {
            return N - 1;
        }

        std::array<char, N> data{};
    };

    /// \brief A type that represents a compile-time string as a type
    /// \tparam str A CtString instance
    template <auto str>
        requires std::is_same_v<std::remove_cvref_t<decltype(str)>, CtString<str.size() + 1>>
    struct TypeString {
        [[nodiscard]] static constexpr const char* data() {
            return str.data.data();
        }

        [[nodiscard]] static constexpr std::size_t size() {
            return str.size();
        }
    };
} // namespace types
