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

    using rva_t = memory::address;

    struct range_t {
        rva_t start;
        rva_t end;

        [[nodiscard]] constexpr std::size_t size() const {
            return (end - start).as<std::size_t>();
        }
    };

    template <traits::default_constructible Ty>
    class Singleton : public base::DefaultCtorDtor<Singleton<Ty>>, public base::NonCopyable<Singleton<Ty>> {
    public:
        [[nodiscard]] static Ty& get() {
            static Ty instance = {};
            return instance;
        }
    };
} // namespace types
