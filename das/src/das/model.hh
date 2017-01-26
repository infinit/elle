#pragma once

#include <elle/attribute.hh>
#include <elle/meta.hh>

#include <das/fwd.hh>

namespace das
{
  template <typename T, typename Fields_>
  class Model
  {
  private:
    template <bool attr, typename A>
    struct FieldTypeHelper
    {
      using type = typename A::template attr_type<T>;
      static
      auto const&
      get(T const& o)
      {
        return A::template attr_get(o);
      }
      static
      auto&
      get(T& o)
      {
        return A::template attr_get(o);
      }
    };

    template <typename A>
    struct FieldTypeHelper<false, A>
    {
      using type =
        std::decay_t<decltype(A::template method_call(std::declval<T>()))>;
      static
      auto
      get(T const& o) -> decltype(A::template method_call(o))
      {
        return A::template method_call(o);
      }
      static
      auto
      get(T& o) -> decltype(A::template method_call(o))
      {
        return A::template method_call(o);
      }
    };

  public:
    template <typename A>
    struct FieldType
      : public FieldTypeHelper<A::template attr_has<T>(), A>
    {};

    using Type = T;
    using Fields = Fields_;
    using Types = typename Fields::template map<FieldType>::type;
  };

  namespace _details
  {
    template <typename T>
    static constexpr
    int
    _default_model(...)
    {
      return 0;
    }

    template <typename T>
    static constexpr
    std::enable_if_exists_t<typename T::Model::Fields, int>
    _default_model(int)
    {
      return 1;
    }

    template <int kind, typename T>
    struct DefaultModelHelper
    {};

    template <typename T>
    struct DefaultModelHelper<1, T>
    {
      using type = typename T::Model;
    };
  }

  template <typename T>
  constexpr inline
  bool
  model_has()
  {
    return _details::_default_model<T>(0);
  }

  template <typename T>
  struct DefaultModel
    : public _details::DefaultModelHelper<_details::_default_model<T>(0), T>
  {};
}

#define DAS_MODEL_DEFAULT(Class, Model)         \
namespace das                                   \
{                                               \
  template <>                                   \
  struct DefaultModel<Class>                    \
  {                                             \
  public:                                       \
    using type =                                \
      ELLE_ATTRIBUTE_STRIP_PARENS(Model);       \
  };                                            \
}
