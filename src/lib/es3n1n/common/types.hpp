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

    template <size_t Footprint>
    struct ct_string_t {
        char data[Footprint]{};

        [[nodiscard]] constexpr size_t size() const {
            return Footprint - 1;
        }

        constexpr ct_string_t(const char (&init)[Footprint]) {
            std::copy_n(init, Footprint, data);
        }
    };

    template <auto str>
        requires std::is_same_v<std::remove_cvref_t<decltype(str)>, ct_string_t<str.size() + 1>>
    struct type_string_t {
        static constexpr const char* data() {
            return str.data;
        }

        static constexpr size_t size() {
            return str.size();
        }
    };
} // namespace types
