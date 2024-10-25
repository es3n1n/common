#pragma once
#include "base.hpp"

template <typename Ty>
struct SaveRestore : public base::NonCopyable {
public:
    constexpr SaveRestore(Ty& ref, Ty new_value) noexcept: ref_(ref), value_(ref) {
        ref_ = new_value;
    }
    ~SaveRestore() {
        ref_ = value_;
    }

private:
    Ty& ref_;
    Ty value_;
};
