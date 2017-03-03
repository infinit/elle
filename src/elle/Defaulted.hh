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

    template <typename ... Args,
              typename Sfinae = std::enable_if_t<
                std::is_constructible<T, Args...>::value, void>>
    explicit
    Defaulted(Args&& ... args)
      : _value(std::forward<Args>(args)...)
      , _set(true)
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
  Defaulted<T>
  defaulted(T&& t)
  {
    return Defaulted<T>(std::forward<T>(t), false);
  }
}
