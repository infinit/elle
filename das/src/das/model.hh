#ifndef DAS_MODEL_CONVENIENCE_HH
# define DAS_MODEL_CONVENIENCE_HH

# include <das/fwd.hh>

namespace das
{
  namespace details
  {
    template <typename T, class C>
    C
    _attribute_pointer_owner(T (C::*));
  }
}

#define DAS_MODEL_DEFINE(Class, Fields, Type)                           \
  typedef das::Object<                                                  \
    Class                                                               \
    DAS_MODEL_DEFINE_(                                                  \
      Class,                                                            \
      BOOST_PP_VARIADIC_TO_SEQ Fields)                                  \
  >                                                                     \
  Type;                                                                 \

#define DAS_MODEL_DEFINE_(Class, Fields)                                \
  DAS_MODEL_DEFINE_SEQ(Class, Fields)                                   \

#define DAS_MODEL_DEFINE_SEQ(Class, Fields)                             \
  BOOST_PP_SEQ_FOR_EACH(DAS_MODEL_DEFINE_HELPER, Class, Fields)         \

#define DAS_MODEL_DEFINE_SEQ_(Class, Fields)                            \
  DAS_MODEL_DEFINE_SEQ__(Class, Fields)                                 \

#define DAS_MODEL_DEFINE_SEQ__(Class, Fields)                           \
  BOOST_PP_SEQ_ENUM(Fields)                                             \

#define DAS_MODEL_DEFINE_HELPER(R, Class, Name)                         \
  , das::Field<decltype(                                                \
      das::details::_attribute_pointer_owner(&Class::Name)),            \
      decltype(Class::Name), &Class::Name>                              \

# define DAS_MODEL_DEFAULT(T, M)                \
  namespace das                                 \
  {                                             \
    template <>                                 \
      struct Das<T>                             \
    {                                           \
      typedef M Model;                          \
    };                                          \
  }

#define DAS_MODEL_FIELDS(Class, Fields)                                 \
  BOOST_PP_SEQ_FOR_EACH(DAS_MODEL_HELPER,                               \
                        Class, BOOST_PP_VARIADIC_TO_SEQ Fields)         \

#define DAS_MODEL(Class, Fields, Name)                                  \
  DAS_MODEL_FIELDS(Class, Fields);                                      \
  DAS_MODEL_DEFINE(Class, Fields, Name);                                \

#define DAS_MODEL_HELPER(R, Class, Name)        \
  DAS_MODEL_FIELD(Class, Name)                  \

# include <das/Object.hh>
# include <das/Collection.hh>
# include <das/Variable.hh>
# include <das/printer.hh>

#endif
