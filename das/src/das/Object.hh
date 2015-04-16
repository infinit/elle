#ifndef DAS_OBJECT_HH
# define DAS_OBJECT_HH

# include <boost/optional.hpp>

# include <elle/serialization/Serializer.hh>

# include <das/Variable.hh>

namespace das
{
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
  };

  template <typename T, typename ... Fields>
  class Object
  {
  public:
    typedef T Model;
    class Update
      : public UpdateHelper<T, Fields...>
    {
    public:
      Update()
      {}

      Update(elle::serialization::SerializerIn& s)
      {
        this->serialize(s);
      }
    };
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
    typedef typename Field<T, M, m>::template Member<UpdateMember> Member;
    void
    serialize(elle::serialization::Serializer& s)
    {
      s.serialize(Field<T, M, m, void>::name, this->*Member::member);
      UpdateHelper<T, Tail ...>::serialize(s);
    }

    void
    apply(T& o) const
    {
      auto const& v = this->*Member::member;
      if (v)
        o.*m = v.get();
      UpdateHelper<T, Tail ...>::apply(o);
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
    };                                                                  \
    template                                                            \
    class Field<Class, decltype(Class::Name), &Class::Name, void>;      \
  }                                                                     \

#endif
