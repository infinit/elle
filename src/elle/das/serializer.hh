#pragma once

#include <type_traits>

#include <elle/serialization/SerializerIn.hh>
#include <elle/serialization/SerializerOut.hh>

#include <elle/das/model.hh>

namespace elle
{
  namespace das
  {
    template <typename T>
    void
    serialize(T const& o, elle::serialization::SerializerOut& s);

    namespace _details
    {
      template <typename T, typename ... Args>
      T
      construct(Args&& ... args)
      {
        return T(std::move(args)...);
      }
    }

    template <typename O, typename M = typename DefaultModel<O>::type>
    struct Serializer
    {
      template <typename T>
      struct Serialize
      {
        using type = int;
        static
        int
        value(O const& o, elle::serialization::SerializerOut& s)
        {
          s.serialize(T::name(), M::template FieldType<T>::get(o));
          return 0;
        }
      };

      template <typename T>
      struct Deserialize
      {
        using type = typename M::template FieldType<T>::type;
        //typename T::template attr_type<O>;
        static
        type
        value(elle::serialization::SerializerIn& s)
        {
          return s.deserialize<type>(T::name());
        }
      };

      template <typename T>
      struct DeserializeAssign
      {
        using type = bool;
        static
        bool
        value(elle::serialization::SerializerIn& s, O& o)
        {
          T::attr_get(o) =
            s.deserialize<typename T::template attr_type<O>>(T::name());
          return false;
        }
      };

      static
      void
      serialize(O const& o, elle::serialization::SerializerOut& s)
      {
        M::Fields::template map<Serialize>::value(o, s);
      }

      static
      O
      deserialize(elle::serialization::SerializerIn& s);

      static
      void
      serialize(std::string const& name,
                O const& o,
                elle::serialization::SerializerOut& s)
      {
        if (auto entry = s.enter(name))
          serialize(o, s);
      }
    };

    namespace _details
    {
      /// Deserialize by constructor.
      template <typename O, typename M>
      std::enable_if_t<
        M::Types::template apply<std::is_constructible, O>::value,
        O>
      deserialize_switch(elle::serialization::SerializerIn& s)
      {
        return std::forward_tuple(
          [] (auto&& ... args) -> O
          {
            return O(std::move(args)...);
          },
          M::Fields::template map<Serializer<O, M>::template Deserialize>::value(s));
      }

      template <typename M>
      struct SetAttr
      {
        using O = typename M::Type;

        template <typename A>
        struct available
        {
          static bool constexpr value =
            std::is_assignable<
            decltype(M::template FieldType<A>::get(std::declval<O&>())),
            typename M::template FieldType<A>::type>::value;
          using type = std::integral_constant<bool, value>;
        };

        template <typename A>
        struct set
        {
          using type = decltype(
            M::template FieldType<A>::get(std::declval<O&>()) =
            std::declval<M::template FieldType<A>::type>());
        };
      };

      /// Deserialize via default construct and fields assignment.
      template <typename O, typename M>
      std::enable_if_t<
        !M::Types::template apply<std::is_constructible, O>::value &&
      M::Fields::template map<SetAttr<M>::template available>::type::template apply<elle::meta::All>::value,
        O>
        deserialize_switch(elle::serialization::SerializerIn& s)
      {
        O res;
        M::Fields::template map<Serializer<O, M>::
                                template DeserializeAssign>::value(s, res);
        return res;
      }
    }

    template <typename T>
    void
    serialize(T const& o, elle::serialization::SerializerOut& s)
    {
      Serializer<T>::serialize(o, s);
    }

    template <typename Model, typename T>
    void
    serialize(T const& o, elle::serialization::SerializerOut& s)
    {
      Serializer<T, Model>::serialize(o, s);
    }

    template <typename T>
    void
    serialize(std::string const& name,
              T const& o,
              elle::serialization::SerializerOut& s)
    {
      Serializer<T>::serialize(name, o, s);
    }

    template <typename Model, typename T>
    void
    serialize(std::string const& name,
              T const& o,
              elle::serialization::SerializerOut& s)
    {
      Serializer<T, Model>::serialize(name, o, s);
    }
  }
}

#define ELLE_DAS_SERIALIZE(Class, ...)                          \
  namespace elle                                                \
  {                                                             \
    namespace serialization                                     \
    {                                                           \
      template <>                                               \
      struct Serialize<Class>                                   \
        : public elle::das::Serializer<Class, ## __VA_ARGS__>   \
      {};                                                       \
    }                                                           \
  }

#include <elle/das/serializer.hxx>
