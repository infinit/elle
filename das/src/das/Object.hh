#ifndef DAS_OBJECT_HH
# define DAS_OBJECT_HH

# include <boost/optional.hpp>

# include <elle/serialization/Serializer.hh>

# include <das/Model.hh>
# include <das/Variable.hh>

namespace das
{
  template <typename O, typename T, T (O::*member)>
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
    apply(T& o)
    {}
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
      s.serialize(Field<T, M, m>::_name, this->*Member::member);
      UpdateHelper<T, Tail ...>::serialize(s);
    }

    void
    apply(T& o)
    {
      auto const& v = this->*Member::member;
      if (v)
        o.*m = v.get();
      UpdateHelper<T, Tail ...>::apply(o);
    }
  };

  template <typename T, typename ... Fields>
  class Object
  {
  public:
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
}

#define DAS_MODEL_FIELD(Class, Name)                                    \
  namespace das                                                         \
  {                                                                     \
    template <>                                                         \
    class Field<Class, decltype(Class::Name), &Class::Name>             \
    {                                                                   \
      public:                                                           \
      constexpr static char const* _name = BOOST_PP_STRINGIZE(Name);    \
      template <template <typename> class Functor>                      \
        struct Member                                                   \
      {                                                                 \
        typedef typename Functor<decltype(Class::Name)>::type Type;     \
        Type Name;                                                      \
        static constexpr Type (Member<Functor>::*member) =              \
          &Member<Functor>::Name;                                       \
      };                                                                \
    };                                                                  \
  }                                                                     \

#endif
