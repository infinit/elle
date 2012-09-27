#ifndef ELLE_ATTRIBUTE_HH
# define ELLE_ATTRIBUTE_HH

# include <type_traits>

# include <boost/preprocessor/seq/cat.hpp>

# define ELLE_ATTRIBUTE(Type, Name)                                     \
  private:                                                              \
  Type BOOST_PP_CAT(_, Name);

# define ELLE_ATTRIBUTE_R_ACCESSOR(Type, Name)                          \
  public:                                                               \
  typename std::remove_cv<                                              \
    typename std::remove_reference<                                     \
      Type                                                              \
    >::type                                                             \
  >::type const&                                                        \
  Name() const                                                          \
  {                                                                     \
    return (this->BOOST_PP_CAT(_, Name));                               \
  }

# define ELLE_ATTRIBUTE_W_ACCESSOR(Type, Name)                          \
  public:                                                               \
  void                                                                  \
  Name(typename std::remove_cv<                                         \
         typename std::remove_reference<                                \
           Type                                                         \
         >::type                                                        \
       >::type const& name)                                             \
  {                                                                     \
    this->BOOST_PP_CAT(_, Name) = name;                                 \
  }

# define ELLE_ATTRIBUTE_X_ACCESSOR(Type, Name)                          \
  public:                                                               \
  typename std::remove_cv<                                              \
    typename std::remove_reference<                                     \
      Type                                                              \
    >::type                                                             \
  >::type&                                                              \
  Name()                                                                \
  {                                                                     \
    return (this->BOOST_PP_CAT(_, Name));                               \
  }

# define ELLE_ATTRIBUTE_R(Type, Name)                                   \
  ELLE_ATTRIBUTE(Type, Name)                                            \
  ELLE_ATTRIBUTE_R_ACCESSOR(Type, Name)

# define ELLE_ATTRIBUTE_W(Type, Name)                                   \
  ELLE_ATTRIBUTE(Type, Name)                                            \
  ELLE_ATTRIBUTE_W_ACCESSOR(Type, Name)

# define ELLE_ATTRIBUTE_X(Type, Name)                                   \
  ELLE_ATTRIBUTE(Type, Name)                                            \
  ELLE_ATTRIBUTE_X_ACCESSOR(Type, Name)

# define ELLE_ATTRIBUTE_RW(Type, Name)                                  \
  ELLE_ATTRIBUTE(Type, Name)                                            \
  ELLE_ATTRIBUTE_R_ACCESSOR(Type, Name)                                 \
  ELLE_ATTRIBUTE_W_ACCESSOR(Type, Name)

# define ELLE_ATTRIBUTE_RX(Type, Name)                                  \
  ELLE_ATTRIBUTE(Type, Name)                                            \
  ELLE_ATTRIBUTE_R_ACCESSOR(Type, Name)                                 \
  ELLE_ATTRIBUTE_X_ACCESSOR(Type, Name)

# define ELLE_ATTRIBUTE_WX(Type, Name)                                  \
  ELLE_ATTRIBUTE(Type, Name)                                            \
  ELLE_ATTRIBUTE_W_ACCESSOR(Type, Name)                                 \
  ELLE_ATTRIBUTE_X_ACCESSOR(Type, Name)

# define ELLE_ATTRIBUTE_RWX(Type, Name)                                 \
  ELLE_ATTRIBUTE(Type, Name)                                            \
  ELLE_ATTRIBUTE_R_ACCESSOR(Type, Name)                                 \
  ELLE_ATTRIBUTE_W_ACCESSOR(Type, Name)                                 \
  ELLE_ATTRIBUTE_X_ACCESSOR(Type, Name)

#endif
