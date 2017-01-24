#pragma once

namespace elle
{
  /// An optional that knows its default value.
  template <typename T>
  class Defaulted
  {
  public:
    Defaulted(T&& def, bool set = false)
      : _value{std::forward<T>(def)}
      , _set{set}
    {}

    Defaulted(Defaulted const&) = default;
    Defaulted& operator=(Defaulted const&) = default;

    /// Override the default value.
    template <typename U>
    Defaulted& operator=(U&& u)
    {
      _value = std::forward<U>(u);
      _set = true;
      return *this;
    }

    /// Whether explicitly defined by the user.
    operator bool() const
    {
      return _set;
    }

    /// The value, readonly.
    T const& get() const
    {
      return _value;
    }

    /// The value, readonly.
    T const& operator*() const
    {
      return get();
    }

    /// A pointer to the value, readonly.
    T const* operator->() const
    {
      return &_value;
    }

  private:
    /// The value.
    T _value;
    /// Whether a value was specified (as opposed to remaining equal
    /// to the initial value).
    bool _set = false;
  };


  template <typename T>
  Defaulted<T>
  defaulted(T&& t)
  {
    return Defaulted<T>(std::forward<T>(t), true);
  }
}
