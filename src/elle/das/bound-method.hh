#pragma once

#include <utility>

#include <elle/Printable.hh>
#include <elle/attribute.hh>
#include <elle/log.hh>

namespace elle
{
  namespace das
  {
    template <typename O, typename S,
              typename P = typename S::template method_signature<O>>
    struct BoundMethod;

    template <typename O, typename S, typename R, typename ... Args>
    struct BoundMethod<O, S, R (Args...)>
      : public Printable::as<BoundMethod<O, S, R (Args...)>>
    {
      BoundMethod(O& o)
        : _object(o)
      {}

      ELLE_LOG_COMPONENT("elle.das.BoundMethod");

      auto
      operator ()(Args ... args) const
      {
        ELLE_TRACE_SCOPE(
          "call %s%s", this, std::tuple<Args const&...>(args...));
        return S::method_call(this->_object, std::forward<Args>(args)...);
      }

      void
      print(std::ostream& o) const
      {
        elle::fprintf(o, "%s.%s", this->_object, S::name());
      }

      ELLE_ATTRIBUTE_R(O&, object);
    };

    template <typename S, typename O>
    BoundMethod<O, S>
    bind_method(O& o)
    {
      return {o};
    }

    template <typename O, typename S>
    BoundMethod<O, S>
    bind_method(O& o, S const&)
    {
      return {o};
    }

    template <typename O, typename R, typename ... Args>
    std::function<R (Args...)>
    bind_method(O const& o, R (O::*m)(Args ... args) const)
    {
      return [&o, m] (Args ... args)
      {
        return (o.*m)(std::forward<Args>(args)...);
      };
    }

    template <typename O, typename R, typename ... Args>
    std::function<R (Args...)>
    bind_method(O& o, R (O::*m)(Args ... args))
    {
      return [&o, m] (Args ... args)
      {
        return (o.*m)(std::forward<Args>(args)...);
      };
    }
  }
}
