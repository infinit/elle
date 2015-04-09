#ifndef DAS_OBJECT_HH
# define DAS_OBJECT_HH

# include <boost/optional.hpp>

# include <elle/serialization/Serializer.hh>

# include <das/Model.hh>

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

  template <typename T, typename M, M (T::*m), typename ... Tail>
  class UpdateHelper<T, Field<T, M, m>, Tail...>
    : public UpdateHelper<T, Tail ...>
  {
  public:
    void
    serialize(elle::serialization::Serializer& s)
    {
      s.serialize(Field<T, M, m>::_name, this->_member);
      UpdateHelper<T, Tail ...>::serialize(s);
    }

    void
    apply(T& o)
    {
      if (this->_member)
        o.*m = this->_member.get();
    }

  private:
    boost::optional<M> _member;
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
    };                                                                  \
  }                                                                     \

#endif
