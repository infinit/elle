#pragma once

#include <elle/attribute.hh>
#include <elle/meta.hh>

#include <elle/das/fwd.hh>

namespace elle
{
  namespace das
  {
    /// A model for a struct.
    ///
    /// The model declares symbols associated with a given struct, so Das can
    /// perform introspection.
    ///
    /// \code{.cc}
    ///
    /// ELLE_DAS_SYMBOL(i);
    /// ELLE_DAS_SYMBOL(s);
    /// ELLE_DAS_SYMBOL(b);
    ///
    /// struct Foo
    /// {
    ///   int i;
    ///   std::string s;
    ///   bool b;
    ///
    ///   using Model = elle::das::Model<Foo,
    ///                                  elle::meta::List<Symbol_i,
    ///                                                   Symbol_s,
    ///                                                   Symbol_s>>;
    /// };
    ///
    /// // You can also use the non-intrusive way.
    /// // using FooModel = elle::das::Model<Foo,
    ///                                      elle::meta::List<Symbol_i,
    ///                                                       Symbol_s,
    ///                                                       Symbol_s>>;
    /// // ELLE_DAS_MODEL_DEFAULT(Foo, FooModel);
    ///
    /// auto f = Foo{};
    /// // Class with a Model offers:
    ///
    /// // - Serialization.
    /// std::stringstream output;
    /// elle::serialization::json::SerializerOut serializer(output, false);
    /// elle::das::serialize(f, serializer);
    /// assert(output.str() == R"JSON({"i":0,"s":"","b":false})JSON");
    ///
    /// // - Default printer
    /// using elle::das::operator <<;
    /// assert(elle::sprintf("%s", f) ==
    ///
    /// \endcode
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
}

#define ELLE_DAS_MODEL_DEFAULT(Class, Model)         \
  namespace elle                                     \
  {                                                  \
    namespace das                                    \
    {                                                \
      template <>                                    \
        struct DefaultModel<Class>                   \
      {                                              \
        public:                                      \
        using type =                                 \
          ELLE_ATTRIBUTE_STRIP_PARENS(Model);        \
      };                                             \
    }                                                \
  }
