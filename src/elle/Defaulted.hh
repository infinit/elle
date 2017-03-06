#pragma once

#include <elle/attribute.hh>

namespace elle
{
  /// An optional that knows its default value.
  template <typename T>
  class Defaulted
  {
  public:
    Defaulted(T def, bool set = true)
      : _value{std::move(def)}
      , _set{set}
    {}

    Defaulted(Defaulted const&) = default;

    Defaulted(Defaulted&&) = default;

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

    /// Whether explicitly defined by the user.
    operator bool() const
    {
      return this->_set;
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
    T const* operator->() const
    {
      return &this->_value;
    }

  private:
    /// The value.
    ELLE_ATTRIBUTE(T, value);
    /// Whether a value was specified (as opposed to remaining equal
    /// to the initial value).
    bool _set = false;
  };

  template <typename T>
  std::ostream&
  operator <<(std::ostream& out, Defaulted<T> const& t)
  {
    elle::fprintf(out, "%s", t.get());
    if (!t)
      elle::fprintf(out, " (default)");
    return out;
  }

  template <typename T>
  Defaulted<T>
  defaulted(T&& t)
  {
    return Defaulted<T>(std::forward<T>(t), false);
  }
}
