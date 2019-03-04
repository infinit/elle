#pragma once

#include <functional>
#include <iostream>

#include <boost/signals2.hpp>

#include <elle/attribute.hh>
#include <elle/log.hh>

namespace elle
{
  template <typename T>
  class VarBase
  {
  public:
    // Disable assigments, or `var = value` can be interpreted as instantiating
    // a new Var on the right hand side.
    void
    operator =(VarBase) = delete;

    void
    on_update(std::function<void (T const&)> f) const
    {
      this->_updated.connect(std::move(f));
    }

    void
    on_update(std::function<void ()> f) const
    {
      this->_updated.connect([f = std::move(f)] (T const&) { f(); });
    }

    ELLE_ATTRIBUTE(boost::signals2::signal<void (T const&)>, updated,
                   mutable, protected);
  };

  template <typename T, bool pod>
  class VarImpl
    : public VarBase<T>
    , public T
  {
  public:
    using T::T;

    template <typename U>
    void
    operator =(U&& u)
    {
      this->T::operator =(std::forward<U>(u));
      this->_updated(*this);
    }

    template <typename U>
    void
    operator +=(U&& u)
    {
      this->T::operator +=(std::forward<U>(u));
      this->_updated(*this);
    }
  };

  template <typename T>
  class VarImpl<T, true>
    : public VarBase<T>
  {
  public:
    VarImpl(T val)
      : _value(std::move(val))
    {}

    template <typename U>
    void
    operator =(U&& u)
    {
      this->_value = std::forward<U>(u);
      this->_updated(**this);
    }

    template <typename U>
    void
    operator +=(U&& u)
    {
      this->_value += std::forward<U>(u);
      this->_updated(**this);
    }

    T&
    operator *()
    {
      return this->_value;
    }

    T const&
    operator *() const
    {
      return *unconst(*this);
    }

    operator T&()
    {
      return **this;
    }

    operator T const& () const
    {
      return **this;
    }

    ELLE_ATTRIBUTE(T, value);
  };

  template <typename T>
  class Var
    : public VarImpl<T, std::is_pod<T>::value>
  {
  public:
    using Super = VarImpl<T, std::is_pod<T>::value>;
    using Super::Super;
  };
}
