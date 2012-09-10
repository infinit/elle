#ifndef ELLE_ATTRIBUTE_HH
# define ELLE_ATTRIBUTE_HH

# include <boost/preprocessor/seq/cat.hpp>

# define ELLE_ATTRIBUTE(Type, Name)             \
  private:                                      \
  Type BOOST_PP_CAT(_, Name);                   \

# define ELLE_ATTRIBUTE_R(Type, Name)           \
  public:                                       \
  Type const&                                   \
  Name() const                                  \
  {                                             \
    return BOOST_PP_CAT(_, Name);               \
  }                                             \
  ELLE_ATTRIBUTE(Type, Name)                    \

#endif
