#pragma once

#include <utility>

#include <elle/Printable.hh>
#include <elle/attribute.hh>
#include <elle/log.hh>

namespace das
{
  template <typename O, typename S>
  struct BoundMethod
    : public elle::Printable::as<BoundMethod<O, S>>
  {
    BoundMethod(O& o)
      : _object(o)
    {}

    template <typename ... Eff>
    auto
    operator ()(Eff&& ... eff)
    {
      ELLE_LOG_COMPONENT("das.BoundMethod");
      ELLE_TRACE_SCOPE("call %s%s", this, std::tuple<Eff const& ...>(eff...));
      return S::method_call(this->_object, std::forward<Eff>(eff)...);
    }

    template <typename ... Eff>
    auto
    operator ()(Eff&& ... eff) const
    {
      ELLE_LOG_COMPONENT("das.BoundMethod");
      ELLE_TRACE_SCOPE("call %s%s", this, std::tuple<Eff const& ...>(eff...));
      return S::method_call(this->_object, std::forward<Eff>(eff)...);
    }

    void
    print(std::ostream& o) const
    {
      elle::fprintf(o, "%s.%s", this->_object, S::name());
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
