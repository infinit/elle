#ifndef ELLE_OPERATOR_HH
# define ELLE_OPERATOR_HH

# include <boost/preprocessor/seq/cat.hpp>

/// Make it easy to define an operator =() method. However, one must
/// take into account that this implementation relies on the presence
/// of a copy constructor which may therefore need to be implemented
/// should the class contain pointers for example.
///
/// !WARNING! This macro-function should never be used unless one
///           knows exactly what he/she is doing.
# define ELLE_OPERATOR_ASSIGNMENT(_type_)                               \
  public:                                                               \
  _type_&                                                               \
  operator =(_type_ const& other)                                       \
  {                                                                     \
    if (this == &other)                                                 \
      return (*this);                                                   \
                                                                        \
    this->~_type_();                                                    \
                                                                        \
    new (this) _type_(other);                                           \
                                                                        \
    return (*this);                                                     \
  }

/// Produce a deleted operator =() so as to make sure one cannot
/// assign such a type.
# define ELLE_OPERATOR_NO_ASSIGNMENT(_type_)                            \
  public:                                                               \
  _type_&                                                               \
  operator =(_type_ const&) = delete;

/// Generate the operator ==() method. Note that this method relies on
/// the operator !=().
# define ELLE_OPERATOR_EQ(_type_)                                       \
  public:                                                               \
  elle::Boolean                                                         \
  operator ==(_type_ const& other) const                                \
  {                                                                     \
    return (!this->operator !=(other));                                 \
  }

/// Generate the operator !=() method. Note that this method relies on
/// the operator ==().
# define ELLE_OPERATOR_NEQ(_type_)                                      \
  public:                                                               \
  elle::Boolean                                                         \
  operator !=(_type_ const& other) const                                \
  {                                                                     \
    return (!this->operator ==(other));                                 \
  }

/// Generate the operator >() method. Note that this method relies on
/// the operator <=().
# define ELLE_OPERATOR_GT(_type_)                                       \
  public:                                                               \
  elle::Boolean                                                         \
  operator >(_type_ const& other) const                                 \
  {                                                                     \
    return (!this->operator <=(other));                                 \
  }

/// Generate the operator >=() method. Note that this method relies on
/// the operator <().
# define ELLE_OPERATOR_GTE(_type_)                                      \
  public:                                                               \
  elle::Boolean                                                         \
  operator >=(_type_ const& other) const                                \
  {                                                                     \
    return (!this->operator <(other));                                  \
  }

/// Generate the operator <() method. Note that this method relies on
/// the operator >=().
# define ELLE_OPERATOR_LT(_type_)                                       \
  public:                                                               \
  elle::Boolean                                                         \
  operator <(_type_ const& other) const                                 \
  {                                                                     \
    return (!this->operator >=(other));                                 \
  }

/// Generate the operator <=() method. Note that this method relies on
/// the operator >().
# define ELLE_OPERATOR_LTE(_type_)                                      \
  public:                                                               \
  elle::Boolean                                                         \
  operator <=(_type_ const& other) const                                \
  {                                                                     \
    return (!this->operator >(other));                                  \
  }

#endif
