#ifndef ELLE_ATTRIBUTE_HH
# define ELLE_ATTRIBUTE_HH

# include <type_traits>

# include <boost/call_traits.hpp>
# include <boost/preprocessor/seq/cat.hpp>

/// Define a private attribute.
# define ELLE_ATTRIBUTE(_type_, _name_)                                 \
  private:                                                              \
  _type_ BOOST_PP_CAT(_, _name_);

/// Define an accessor returning a const reference on the attribute.
# define ELLE_ATTRIBUTE_r_ACCESSOR(_type_, _name_)                      \
  public:                                                               \
  typename boost::call_traits<_type_>::param_type                       \
  _name_() const                                                        \

/// Define and implement an accessor returning a const reference on the attribute.
# define ELLE_ATTRIBUTE_R_ACCESSOR(_type_, _name_)                      \
  ELLE_ATTRIBUTE_r_ACCESSOR(_type_, _name_)                             \
  {                                                                     \
    return (this->BOOST_PP_CAT(_, _name_));                             \
  }

/// Define an accessor enabling one to set the attribute.
# define ELLE_ATTRIBUTE_w_ACCESSOR(_type_, _name_)                      \
  public:                                                               \
  void                                                                  \
  _name_(typename boost::call_traits<_type_>::param_type name)          \

/// Define and implement an accessor enabling one to set the attribute.
# define ELLE_ATTRIBUTE_W_ACCESSOR(_type_, _name_)                      \
  ELLE_ATTRIBUTE_w_ACCESSOR(_type_, _name_)                             \
  {                                                                     \
    this->BOOST_PP_CAT(_, _name_) = name;                               \
  }

/// Define an accessor returning a reference on the attribute i.e
/// enabling one to potentially modify the attribute outside the class.
# define ELLE_ATTRIBUTE_x_ACCESSOR(_type_, _name_)                      \
  public:                                                               \
  typename boost::call_traits<_type_>::reference                        \
  _name_()                                                              \

/// Define and implement an accessor returning a reference on the attribute i.e
/// enabling one to potentially modify the attribute outside the class.
# define ELLE_ATTRIBUTE_X_ACCESSOR(_type_, _name_)                      \
  ELLE_ATTRIBUTE_x_ACCESSOR(_type_, _name_)                             \
  {                                                                     \
    return (this->BOOST_PP_CAT(_, _name_));                             \
  }

# define ELLE_ATTRIBUTE_RW_ACCESSORS(_type_, _name_)                    \
  ELLE_ATTRIBUTE_R_ACCESSOR(_type_, _name_)                             \
  ELLE_ATTRIBUTE_W_ACCESSOR(_type_, _name_)

# define ELLE_ATTRIBUTE_RX_ACCESSORS(_type_, _name_)                    \
  ELLE_ATTRIBUTE_R_ACCESSOR(_type_, _name_)                             \
  ELLE_ATTRIBUTE_X_ACCESSOR(_type_, _name_)

# define ELLE_ATTRIBUTE_WX_ACCESSORS(_type_, _name_)                    \
  ELLE_ATTRIBUTE_W_ACCESSOR(_type_, _name_)                             \
  ELLE_ATTRIBUTE_X_ACCESSOR(_type_, _name_)

# define ELLE_ATTRIBUTE_RWX_ACCESSORS(_type_, _name_)                   \
  ELLE_ATTRIBUTE_R_ACCESSOR(_type_, _name_)                             \
  ELLE_ATTRIBUTE_W_ACCESSOR(_type_, _name_)                             \
  ELLE_ATTRIBUTE_X_ACCESSOR(_type_, _name_)

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
  ELLE_ATTRIBUTE_RW_ACCESSORS(_type_, _name_)

# define ELLE_ATTRIBUTE_Rw(_type_, _name_)                              \
  ELLE_ATTRIBUTE(_type_, _name_)                                        \
  ELLE_ATTRIBUTE_R_ACCESSOR(_type_, _name_)                             \
  ELLE_ATTRIBUTE_w_ACCESSOR(_type_, _name_)                             \

# define ELLE_ATTRIBUTE_RX(_type_, _name_)                              \
  ELLE_ATTRIBUTE(_type_, _name_)                                        \
  ELLE_ATTRIBUTE_RX_ACCESSORS(_type_, _name_)

# define ELLE_ATTRIBUTE_WX(_type_, _name_)                              \
  ELLE_ATTRIBUTE(_type_, _name_)                                        \
  ELLE_ATTRIBUTE_WX_ACCESSORS(_type_, _name_)

# define ELLE_ATTRIBUTE_RWX(_type_, _name_)                             \
  ELLE_ATTRIBUTE(_type_, _name_)                                        \
  ELLE_ATTRIBUTE_RWX_ACCESSORS(_type_, _name_)

# define ELLE_ATTRIBUTE_RP(_type_, _name_, _property_)                  \
  ELLE_ATTRIBUTE(_property_ _type_, _name_)                             \
  ELLE_ATTRIBUTE_R_ACCESSOR(_type_, _name_)

# define ELLE_ATTRIBUTE_WP(_type_, _name_, _property_)                  \
  ELLE_ATTRIBUTE(_property_ _type_, _name_)                             \
  ELLE_ATTRIBUTE_W_ACCESSOR(_type_, _name_)

# define ELLE_ATTRIBUTE_XP(_type_, _name_, _property_)                  \
  ELLE_ATTRIBUTE(_property_ _type_, _name_)                             \
  ELLE_ATTRIBUTE_X_ACCESSOR(_type_, _name_)

# define ELLE_ATTRIBUTE_RWP(_type_, _name_, _property_)                 \
  ELLE_ATTRIBUTE(_property_ _type_, _name_)                             \
  ELLE_ATTRIBUTE_RW_ACCESSORS(_type_, _name_)

# define ELLE_ATTRIBUTE_RXP(_type_, _name_, _property_)                 \
  ELLE_ATTRIBUTE(_property_ _type_, _name_)                             \
  ELLE_ATTRIBUTE_RX_ACCESSORS(_type_, _name_)

# define ELLE_ATTRIBUTE_WXP(_type_, _name_, _property_)                 \
  ELLE_ATTRIBUTE(_property_ _type_, _name_)                             \
  ELLE_ATTRIBUTE_WX_ACCESSORS(_type_, _name_)

# define ELLE_ATTRIBUTE_RWXP(_type_, _name_, _property_)                \
  ELLE_ATTRIBUTE(_property_ _type_, _name_)                             \
  ELLE_ATTRIBUTE_RWX_ACCESSORS(_type_, _name_)

#endif
