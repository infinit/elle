#ifndef ELLE_ATTRIBUTE_HH
# define ELLE_ATTRIBUTE_HH

# include <type_traits>

# include <boost/preprocessor/seq/cat.hpp>

# define ELLE_ATTRIBUTE(Type, Name)             \
  private:                                      \
  Type BOOST_PP_CAT(_, Name);                   \

# define ELLE_ATTRIBUTE_R(Type, Name)                   \
  ELLE_ATTRIBUTE(Type, Name)                            \
  ELLE_ATTRIBUTE_R_(Type, Name)                         \

# define ELLE_ATTRIBUTE_X(Type, Name)                   \
  ELLE_ATTRIBUTE(Type, Name)                            \
  ELLE_ATTRIBUTE_X_(Type, Name)                         \

# define ELLE_ATTRIBUTE_RX(Type, Name)                   \
  ELLE_ATTRIBUTE(Type, Name)                             \
  ELLE_ATTRIBUTE_R_(Type, Name)                          \
  ELLE_ATTRIBUTE_X_(Type, Name)                          \

# define ELLE_ATTRIBUTE_R_(Type, Name)                  \
  public:                                               \
  typename std::remove_reference<Type>::type const&     \
  Name() const                                          \
  {                                                     \
    return BOOST_PP_CAT(_, Name);                       \
  }                                                     \

# define ELLE_ATTRIBUTE_X_(Type, Name)                  \
  public:                                               \
  typename std::remove_reference<Type>::type&           \
  Name()                                                \
  {                                                     \
    return BOOST_PP_CAT(_, Name);                       \
  }                                                     \

#endif
