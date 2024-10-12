#pragma once
#include "base.hpp"
#include "macros.hpp"
#include <type_traits>

/// \brief A class that holds a callable object and executes it upon destruction.
/// \tparam Callable The type of the callable object.
template <typename Callable>
class DeferHolder : public base::NonCopyable {
public:
    /// \brief Constructor that takes a callable object.
    /// \param callable The callable object to be executed upon destruction.
    explicit DeferHolder(Callable&& callable) noexcept: callable_(std::forward<Callable>(callable)) { }

    /// \brief Destructor that executes the stored callable object.
    ~DeferHolder() {
        callable_();
    }

private:
    Callable callable_; ///< The stored callable object.
};

/// \brief A helper class for creating deferred execution objects.
class Defer : public base::NonCopyable {
public:
    /// \brief Default constructor.
    constexpr Defer() noexcept = default;

    /// \brief Default destructor.
    constexpr ~Defer() noexcept = default;

    /// \brief Overloaded modulus operator to create a DeferHolder object.
    /// \tparam Callable The type of the callable object.
    /// \param cb The callable object to be deferred.
    /// \return A DeferHolder object containing the callable.
    template <typename Callable>
    DeferHolder<Callable> operator%(Callable&& cb) {
        return DeferHolder<Callable>{std::forward<Callable>(cb)};
    }
};

/// \brief Create a deferred execution object.
/// \note This macro creates a unique variable name for each use.
#define defer auto COMMON_CAT(_defer_instance_, __LINE__) = Defer{} % [&]()
