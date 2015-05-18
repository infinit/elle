#ifndef DAS_OBJECT_HH
# define DAS_OBJECT_HH

# include <boost/optional.hpp>

# include <elle/Backtrace.hh>
# include <elle/serialization/Serializer.hh>

# include <das/Variable.hh>

namespace das
{
  template <typename T, typename K, K (T::*key), typename Model>
  class Collection;

  template <typename O, typename T, T (O::*member), typename Model = void>
  class Field
  {};

  template <typename T, typename ... Fields>
  class UpdateHelper
  {
  public:
    void
    serialize(elle::serialization::Serializer& s)
    {}

    void
    apply(T& o) const
    {}

    void
    print(std::ostream& s, bool comma) const
    {}
  };

  template <typename T, typename ... Fields>
  struct FieldHelper
  {
    template <template <typename> class F, typename ... Args>
    static
    void
    each_field(T const&, Args&& ...)
    {}
  };

  template <typename T, typename Head, typename ... Tail>
  struct FieldHelper<T, Head, Tail...>
  {
    template <template <typename> class F, typename ... Args>
    static
    void
    each_field(T& o, Args&& ... args)
    {
      F<Head>::apply(o, std::forward<Args>(args)...);
      FieldHelper<T, Tail...>::template each_field<F>(
        o, std::forward<Args>(args)...);
    }

    template <template <typename> class F, typename ... Args>
    static
    void
    each_field(T const& o, Args&& ... args)
    {
      F<Head>::apply(o, std::forward<Args>(args)...);
      FieldHelper<T, Tail...>::template each_field<F>(
        o, std::forward<Args>(args)...);
    }
  };

  template <typename T, typename ... Fields>
  class Object
  {
  public:
    typedef T Model;
    class Update
      : public UpdateHelper<T, Fields...>
      , public elle::Printable
    {
    public:
      typedef UpdateHelper<T, Fields...> Super;

      Update()
      {}

      Update(elle::serialization::SerializerIn& s)
      {
        this->serialize(s);
      }

      virtual
      void
      print(std::ostream& s) const override
      {
        elle::fprintf(s, "%s::Update(", elle::demangle(typeid(T).name()));
        Super::print(s, false);
        s << ")";
      }
    };

    template <template <typename> class F, typename ... Args>
    static
    void
    each_field(T& o, Args&& ... args)
    {
      FieldHelper<T, Fields...>::template each_field<F>(
        o, std::forward<Args>(args)...);
    }

    template <template <typename> class F, typename ... Args>
    static
    void
    each_field(T const& o, Args&& ... args)
    {
      FieldHelper<T, Fields...>::template each_field<F>(
        o, std::forward<Args>(args)...);
    }
  };

  template <typename T>
  struct
  UpdateMember
  {
    typedef boost::optional<T> type;
  };

  template <typename T>
  struct
  UpdateMember<das::Variable<T>>
  {
    typedef boost::optional<T> type;
  };

  template <typename T, typename M, M (T::*m), typename ... Tail>
  class UpdateHelper<T, Field<T, M, m>, Tail...>
    : public UpdateHelper<T, Tail ...>
    , public Field<T, M, m>::template Member<UpdateMember>
  {
  public:
    typedef UpdateHelper<T, Tail ...> Super;
    typedef typename Field<T, M, m>::template Member<UpdateMember> Member;

    inline
    static
    char const*
    field_name()
    {
      return Field<T, M, m, void>::name;
    }

    void
    serialize(elle::serialization::Serializer& s)
    {
      s.serialize(field_name(), this->*Member::member);
      Super::serialize(s);
    }

    void
    apply(T& o) const
    {
      auto const& v = this->*Member::member;
      if (v)
        o.*m = v.get();
      Super::apply(o);
    }

    void
    print(std::ostream& s, bool comma) const
    {
      auto const& v = this->*Member::member;
      if (v)
      {
        if (comma)
          s << ", ";
        elle::fprintf(s, "%s = %s", field_name(), v.get());
        Super::print(s, true);
      }
      else
        Super::print(s, comma);
    }
  };

  template <typename T>
  struct ReplaceMember
  {
    template <typename Ignored>
    struct Functor
    {
      typedef boost::optional<T> type;
    };
  };

  template <typename T, typename M, M (T::*m), typename Model,
            typename ... Tail, typename ... Fields>
  class UpdateHelper<T, Field<T, M, m, das::Object<Model, Fields...>>, Tail...>
    : public UpdateHelper<T, Tail ...>
    , public Field<T, M, m>::template Member<ReplaceMember<typename das::Object<Model, Fields...>::Update>::template Functor>
  {
  public:
    typedef typename Field<T, M, m>::template Member<ReplaceMember<typename das::Object<Model, Fields...>::Update>::template Functor> Member;
    void
    serialize(elle::serialization::Serializer& s)
    {
      s.serialize(Field<T, M, m>::name, this->*Member::member);
      UpdateHelper<T, Tail ...>::serialize(s);
    }

    void
    apply(T& o) const
    {
      auto const& v = this->*Member::member;
      if (v)
        v.get().apply(o.*m);
      UpdateHelper<T, Tail ...>::apply(o);
    }
  };

  template <typename T, typename M, M (T::*m), typename ... Tail,
            typename Element, typename Key, Key (Element::*key), typename Model>
  class UpdateHelper<T, Field<T, M, m, das::Collection<Element, Key, key, Model>>, Tail...>
    : public UpdateHelper<T, Tail ...>
    , public Field<T, M, m>::template Member<ReplaceMember<typename das::Collection<Element, Key, key, Model>::Update>::template Functor>
  {
  public:
    typedef typename Field<T, M, m>::template Member<ReplaceMember<typename das::Collection<Element, Key, key, Model>::Update>::template Functor> Member;
    void
    serialize(elle::serialization::Serializer& s)
    {
      s.serialize(Field<T, M, m>::name, this->*Member::member);
      UpdateHelper<T, Tail ...>::serialize(s);
    }

    void
    apply(T& o) const
    {
      auto const& v = this->*Member::member;
      if (v)
        v.get().apply(o.*m);
      UpdateHelper<T, Tail ...>::apply(o);
    }
  };
}

#define DAS_MODEL_FIELD(Class, Name)                                    \
  namespace das                                                         \
  {                                                                     \
    template <>                                                         \
    class Field<Class, decltype(Class::Name), &Class::Name, void>       \
    {                                                                   \
    public:                                                             \
      constexpr static char const* name = BOOST_PP_STRINGIZE(Name);     \
      template <template <typename> class Functor>                      \
        struct Member                                                   \
      {                                                                 \
        typedef typename Functor<decltype(Class::Name)>::type Type;     \
        Type Name;                                                      \
        static constexpr Type (Member<Functor>::*member) =              \
          &Member<Functor>::Name;                                       \
      };                                                                \
                                                                        \
      static                                                            \
      decltype(((Class*)(nullptr))->Name)&                              \
      get(Class& o)                                                     \
      {                                                                 \
        return o.Name;                                                  \
      };                                                                \
                                                                        \
      static                                                            \
      decltype(((Class*)(nullptr))->Name) const&                        \
      get(Class const& o)                                               \
      {                                                                 \
        return o.Name;                                                  \
      };                                                                \
    };                                                                  \
    template                                                            \
    class Field<Class, decltype(Class::Name), &Class::Name, void>;      \
  }                                                                     \

#endif
