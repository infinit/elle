#pragma once

#include <elle/attribute.hh>
#include <elle/print.hh>

namespace elle
{
  /// An optional that knows its default value.
  ///
  /// The value returned is read-only, to keep consistency.
  ///
  /// \code{.cc}
  ///
  /// auto d = Defaulted<bool>(true);
  /// assert(!d); // d was not set.
  /// assert(d.get()); // d's value is true.
  /// d = false;
  /// assert(d); // d was set.
  /// assert(!d.get()); // d's value is now false.
  ///
  /// \endcode
  template <typename T>
  class Defaulted
  {
  public:
    Defaulted(T def, bool set = true)
      : _value{std::move(def)}
      , _set{set}
    {}

    Defaulted&
    operator=(Defaulted const&) = default;

    /// Override the default value.
    template <typename U>
    Defaulted&
    operator=(U&& u)
    {
      this->_value = std::forward<U>(u);
      this->_set = true;
      return *this;
    }

    /// The value, readonly.
    T const&
    get() const
    {
      return this->_value;
    }

    /// The value, readonly.
    T const&
    operator*() const
    {
      return this->get();
    }

    /// A pointer to the value, readonly.
    T const*
    operator->() const
    {
      return &this->_value;
    }

    /// Conversion to the value.
    operator T const&() const
    {
      return this->_value;
    }

  private:
    /// The value.
    ELLE_ATTRIBUTE(T, value);
    /// Whether a value was specified (as opposed to remaining equal
    /// to the initial value).
    ELLE_ATTRIBUTE_R(bool, set);
  };

  template <typename T>
  std::ostream&
  operator <<(std::ostream& out, Defaulted<T> const& t)
  {
    elle::print(out, "%s", t.get());
    if (repr(out) && !t.set())
      elle::print(out, " (default)");
    return out;
  }

  template <typename T>
  Defaulted<T>
  defaulted(T&& t)
  {
    return Defaulted<T>(std::forward<T>(t), false);
  }
}
