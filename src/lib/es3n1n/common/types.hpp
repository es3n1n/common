#pragma once
#include "base.hpp"
#include "memory/address.hpp"
#include "traits.hpp"
#include <array>

namespace types {
    template <class... Args>
    constexpr auto to_array(Args&&... args) {
        return std::array<std::common_type_t<Args...>, sizeof...(Args)>{std::forward<Args>(args)...};
    }

    template <typename Ty>
    class Singleton : public base::NonCopyable {
    public:
        [[nodiscard]] static Ty& get() {
            static Ty instance = {};
            return instance;
        }
    };

    template <size_t N>
    struct ct_string_t {
        std::array<char, N> data{};

        [[nodiscard]] constexpr size_t size() const {
            return N - 1;
        }

        constexpr /* implicit */ ct_string_t(const char (&init)[N]) {
            std::copy_n(init, N, data.begin());
        }
    };

    template <auto str>
        requires std::is_same_v<std::remove_cvref_t<decltype(str)>, ct_string_t<str.size() + 1>>
    struct type_string_t {
        static constexpr const char* data() {
            return str.data.data();
        }

        static constexpr size_t size() {
            return str.size();
        }
    };
} // namespace types
