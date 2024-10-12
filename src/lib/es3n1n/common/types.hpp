#pragma once
#include <algorithm>
#include <array>
#include <type_traits>

#include "base.hpp"
#include "memory/address.hpp"
#include "strong_integral.hpp"
#include "traits.hpp"

namespace types {
    /// \brief Converts a list of arguments into an std::array
    /// \tparam Args Variadic template parameter for the argument types
    /// \param args The arguments to convert into an array
    /// \return An std::array containing the provided arguments
    template <typename... Args>
    [[nodiscard]] constexpr auto to_array(Args&&... args) {
        return std::array<std::common_type_t<Args...>, sizeof...(Args)>{std::forward<Args>(args)...};
    }

    /// \brief A template class for creating singleton objects
    /// \tparam Ty The type of the singleton object
    template <typename Ty>
    class Singleton : public base::NonCopyable {
    public:
        /// \brief Get the singleton instance
        /// \return A reference to the singleton instance
        [[nodiscard]] static Ty& get() {
            static Ty instance = {};
            return instance;
        }

    protected:
        /// \brief Protected constructor to prevent direct instantiation
        Singleton() = default;
    };

    /// \brief A compile-time string class
    /// \tparam N The size of the string including the null terminator
    template <std::size_t N>
    struct CtString {
        std::array<char, N> data{};

        /// \brief Get the size of the string (excluding null terminator)
        /// \return The size of the string
        [[nodiscard]] constexpr std::size_t size() const {
            return N - 1;
        }

        /// \brief Implicit constructor from a char array
        /// \param init The char array to initialize the CtString with
        constexpr /* implicit */ CtString(const char (&init)[N]) {
            std::copy_n(init, N, data.begin());
        }
    };

    /// \brief A type that represents a compile-time string as a type
    /// \tparam str A CtString instance
    template <auto str>
        requires std::is_same_v<std::remove_cvref_t<decltype(str)>, CtString<str.size() + 1>>
    struct TypeString {
        /// \brief Get the underlying string data
        /// \return A pointer to the string data
        [[nodiscard]] static constexpr const char* data() {
            return str.data.data();
        }

        /// \brief Get the size of the string (excluding null terminator)
        /// \return The size of the string
        [[nodiscard]] static constexpr std::size_t size() {
            return str.size();
        }
    };
} // namespace types
