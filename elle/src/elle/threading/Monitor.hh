#ifndef ELLE_THREADING_MONITOR_HH
# define ELLE_THREADING_MONITOR_HH

# include <mutex>

namespace elle
{
  namespace threading
  {
    /// @brief Protect any data from concurrent accesses.
    ///
    /// The monitor class is a wrapper that contains a value of type @a T and @a
    /// mutex of type MutexType that should verify the 'BasicLockable' concept.
    /// This way, you cannot access the stored value without locking first.
    ///
    /// @note You don't need to provide a recursive mutex type.
    ///
    /// @see http://en.cppreference.com/w/cpp/concept/BasicLockable
    template <typename T, typename MutexType = std::mutex>
    struct Monitor;

    /// @typedef Monitor::value_type
    /// The wrapped value type

    /// @typedef Monitor::mutex_type
    /// The mutex type

    /// @fn Monitor::Monitor()
    /// The monitor class is default constructible.

    /// @fn Monitor::Monitor(value_type const& value)
    /// Construct a monitor copy @a value of type @a value_type

    /// @fn Monitor::Monitor(value_type&& value)
    /// Construct a monitor moving @a value of type @a value_type

    /// @fn T* Monitor::operator ->()
    /// @fn T const* Monitor::operator ->() const
    /// The operator "->" gives access to the wrapped value members
    /// @note The object is locked while the access to that member is made.

    /// @fn Ret Monitor::operator ()(Callable<Ret(T&)>)
    /// @fn Ret Monitor::operator ()(Callable<Ret(T const&)>) const
    /// Access to the wrapped value in a callable.

  }
}

# include "Monitor.hxx"

#endif
