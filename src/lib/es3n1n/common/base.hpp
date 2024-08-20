#pragma once

namespace base {
    template <typename Derived>
    class DefaultCtor {
    public:
        constexpr DefaultCtor() = default;
    };

    template <typename Derived>
    class DefaultDtor {
    public:
        ~DefaultDtor() = default;
    };

    template <typename Derived>
    class DefaultCtorDtor : public DefaultCtor<Derived>, public DefaultDtor<Derived> {
    public:
    };

    template <typename Derived>
    class NonCopyable {
    public:
        NonCopyable(const NonCopyable&) = delete;
        NonCopyable& operator=(const NonCopyable&) = delete;
        NonCopyable(NonCopyable&&) = delete;
        NonCopyable& operator=(NonCopyable&&) = delete;
    };

    template <typename Derived>
    class DefaultCopy {
    public:
        DefaultCopy(const DefaultCopy&) = default;
        DefaultCopy& operator=(const DefaultCopy&) = default;
        DefaultCopy(DefaultCopy&&) noexcept = default;
        DefaultCopy& operator=(DefaultCopy&&) noexcept = default;
    };
} // namespace base
