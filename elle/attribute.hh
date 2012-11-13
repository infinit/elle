#ifndef ELLE_ATTRIBUTE_HH
# define ELLE_ATTRIBUTE_HH

# include <type_traits>

# include <boost/preprocessor/seq/cat.hpp>

/// Define a private attribute.
# define ELLE_ATTRIBUTE(_type_, _name_)                                 \
  private:                                                              \
  _type_ BOOST_PP_CAT(_, _name_);

/// Define an accessor returning a const reference on the attribute.
# define ELLE_ATTRIBUTE_R_ACCESSOR(_type_, _name_)                      \
  public:                                                               \
  typename std::remove_cv<                                              \
    typename std::remove_reference<                                     \
      _type_                                                            \
    >::type                                                             \
  >::type const&                                                        \
  _name_() const                                                        \
  {                                                                     \
    return (this->BOOST_PP_CAT(_, _name_));                             \
  }

/// Define an accessor enabling one to set the attribute.
# define ELLE_ATTRIBUTE_W_ACCESSOR(_type_, _name_)                      \
  public:                                                               \
  void                                                                  \
  _name_(typename std::remove_cv<                                       \
         typename std::remove_reference<                                \
           _type_                                                       \
         >::type                                                        \
       >::type const& name)                                             \
  {                                                                     \
    this->BOOST_PP_CAT(_, _name_) = name;                               \
  }

/// Define an accessor returning a reference on the attribute i.e
/// enabling one to potentially modify the attribute outside the class.
# define ELLE_ATTRIBUTE_X_ACCESSOR(_type_, _name_)                      \
  public:                                                               \
  typename std::remove_cv<                                              \
    typename std::remove_reference<                                     \
      _type_                                                            \
    >::type                                                             \
  >::type&                                                              \
  _name_()                                                              \
  {                                                                     \
    return (this->BOOST_PP_CAT(_, _name_));                             \
  }

# define ELLE_ATTRIBUTE_R(_type_, _name_)                               \
  ELLE_ATTRIBUTE(_type_, _name_)                                        \
  ELLE_ATTRIBUTE_R_ACCESSOR(_type_, _name_)

# define ELLE_ATTRIBUTE_W(_type_, _name_)                               \
  ELLE_ATTRIBUTE(_type_, _name_)                                        \
  ELLE_ATTRIBUTE_W_ACCESSOR(_type_, _name_)

# define ELLE_ATTRIBUTE_X(_type_, _name_)                               \
  ELLE_ATTRIBUTE(_type_, _name_)                                        \
  ELLE_ATTRIBUTE_X_ACCESSOR(_type_, _name_)

# define ELLE_ATTRIBUTE_RW(_type_, _name_)                              \
  ELLE_ATTRIBUTE(_type_, _name_)                                        \
  ELLE_ATTRIBUTE_R_ACCESSOR(_type_, _name_)                             \
  ELLE_ATTRIBUTE_W_ACCESSOR(_type_, _name_)

# define ELLE_ATTRIBUTE_RX(_type_, _name_)                              \
  ELLE_ATTRIBUTE(_type_, _name_)                                        \
  ELLE_ATTRIBUTE_R_ACCESSOR(_type_, _name_)                             \
  ELLE_ATTRIBUTE_X_ACCESSOR(_type_, _name_)

# define ELLE_ATTRIBUTE_WX(_type_, _name_)                              \
  ELLE_ATTRIBUTE(_type_, _name_)                                        \
  ELLE_ATTRIBUTE_W_ACCESSOR(_type_, _name_)                             \
  ELLE_ATTRIBUTE_X_ACCESSOR(_type_, _name_)

# define ELLE_ATTRIBUTE_RWX(_type_, _name_)                             \
  ELLE_ATTRIBUTE(_type_, _name_)                                        \
  ELLE_ATTRIBUTE_R_ACCESSOR(_type_, _name_)                             \
  ELLE_ATTRIBUTE_W_ACCESSOR(_type_, _name_)                             \
  ELLE_ATTRIBUTE_X_ACCESSOR(_type_, _name_)

#endif
