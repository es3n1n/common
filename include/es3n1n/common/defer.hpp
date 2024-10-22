#pragma once
#include "base.hpp"
#include "macros.hpp"
#include <utility>

template <typename Callable>
class DeferHolder : public base::NonCopyable {
public:
    explicit DeferHolder(Callable&& callable) noexcept: callable_(std::forward<Callable>(callable)) { }

    ~DeferHolder() {
        callable_();
    }

private:
    Callable callable_;
};

class Defer : public base::NonCopyable {
public:
    constexpr Defer() noexcept = default;
    constexpr ~Defer() noexcept = default;

    template <typename Callable>
    DeferHolder<Callable> operator%(Callable&& cb) {
        return DeferHolder<Callable>{std::forward<Callable>(cb)};
    }
};

/// \brief Create a deferred execution object.
/// \note This macro creates a unique variable name for each use.
#define defer auto COMMON_CAT(_defer_instance_, __LINE__) = Defer{} % [&]()
