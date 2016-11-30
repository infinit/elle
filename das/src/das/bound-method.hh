#pragma once

#include <utility>

#include <elle/attribute.hh>

namespace das
{
  template <typename O, typename S>
  struct BoundMethod
  {
    BoundMethod(O& o)
      : _object(o)
    {}

    template <typename ... Eff>
    auto
    operator ()(Eff&& ... eff)
    {
      return S::method_call(this->_object, std::forward<Eff>(eff)...);
    }

    template <typename ... Eff>
    auto
    operator ()(Eff&& ... eff) const
    {
      return S::method_call(this->_object, std::forward<Eff>(eff)...);
    }

    ELLE_ATTRIBUTE_R(O&, object);
  };

  template <typename O, typename S>
  BoundMethod<O, S>
  bind_method(O& o)
  {
    return BoundMethod<O, S>(o);
  }

  template <typename O, typename S>
  BoundMethod<O, S>
  bind_method(O& o, S&)
  {
    return BoundMethod<O, S>(o);
  }
}
