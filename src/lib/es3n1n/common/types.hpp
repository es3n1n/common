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
} // namespace types
