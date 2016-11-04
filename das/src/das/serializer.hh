#ifndef DAS_SERIALIZER_HH
# define DAS_SERIALIZER_HH

# include <type_traits>

# include <elle/serialization/SerializerIn.hh>
# include <elle/serialization/SerializerOut.hh>

# include <das/model.hh>

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
        s.serialize(T::name(), T::attr_get(o));
        return 0;
      }
    };

    template <typename T>
    struct Deserialize
    {
      using type = typename T::template attr_type<O>::type;
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
          s.deserialize<typename T::template attr_type<O>::type>(T::name());
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
    typename std::enable_if<
      M::Types::template apply<std::is_constructible, O>::type::value,
      O>::type
    deserialize_switch(elle::serialization::SerializerIn& s)
    {
      return std::forward_tuple(
        [] (auto&& ... args) -> O
        {
          return O(std::move(args)...);
        },
        M::Fields::template map<Serializer<O, M>::template Deserialize>::value(s));
    }

    template <typename O>
    struct SetAttr
    {
      template <typename A>
      struct set
      {
        using type = decltype(
          A::attr_get(std::declval<O&>()) =
          std::declval<typename A::template attr_type<O>::type>());
      };
    };

    /// Deserialize via default construct and fields assignment.
    template <typename O, typename M>
    typename std::enable_if<
      !M::Types::template apply<std::is_constructible, O>::type::value &&
      sizeof(typename M::Fields::template map<SetAttr<O>::template set>::type) >= 0,
      O>::type
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

# define DAS_SERIALIZE(Class, ...)                               \
  namespace elle                                                \
  {                                                             \
    namespace serialization                                     \
    {                                                           \
      template <>                                               \
        struct Serialize<Class>                                 \
          : public das::Serializer<Class, ## __VA_ARGS__>       \
      {};                                                       \
    }                                                           \
  }                                                             \

# include <das/serializer.hxx>

#endif
