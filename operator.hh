#ifndef ELLE_OPERATOR_HH
# define ELLE_OPERATOR_HH

# include <boost/preprocessor/seq/cat.hpp>

/// Makes it easy to define an operator =() method. However, one must
/// take into account that this implementation relies on the presence
/// of a copy constructor which may therefore need to be implemented
/// should the class contain pointers for example.
# define ELLE_OPERATOR_ASSIGNMENT(Type)                                 \
  public:                                                               \
  Type&                                                                 \
  operator =(Type const& other)                                         \
  {                                                                     \
    if (this == &other)                                                 \
      return (*this);                                                   \
                                                                        \
    this->~Type();                                                      \
                                                                        \
    new (this) Type(other);                                             \
                                                                        \
    return (*this);                                                     \
  }

/// Generates the operator ==() method. Note that this method relies on
/// the operator !=().
# define ELLE_OPERATOR_EQ(Type)                                         \
  public:                                                               \
  elle::Boolean                                                         \
  operator ==(Type const& other) const                                  \
  {                                                                     \
    return (!this->operator !=(other));                                 \
  }

/// Generates the operator !=() method. Note that this method relies on
/// the operator ==().
# define ELLE_OPERATOR_NEQ(Type)                                       \
  public:                                                               \
  elle::Boolean                                                         \
  operator !=(Type const& other) const                                  \
  {                                                                     \
    return (!this->operator ==(other));                                 \
  }

/// Generates the operator >() method. Note that this method relies on
/// the operator <=().
# define ELLE_OPERATOR_GT(Type)                                         \
  public:                                                               \
  elle::Boolean                                                         \
  operator >(Type const& other) const                                   \
  {                                                                     \
    return (!this->operator <=(other));                                 \
  }

/// Generates the operator >=() method. Note that this method relies on
/// the operator <().
# define ELLE_OPERATOR_GTE(Type)                                        \
  public:                                                               \
  elle::Boolean                                                         \
  operator >=(Type const& other) const                                  \
  {                                                                     \
    return (!this->operator <(other));                                  \
  }

/// Generates the operator <() method. Note that this method relies on
/// the operator >=().
# define ELLE_OPERATOR_LT(Type)                                         \
  public:                                                               \
  elle::Boolean                                                         \
  operator <(Type const& other) const                                   \
  {                                                                     \
    return (!this->operator >=(other));                                 \
  }

/// Generates the operator <=() method. Note that this method relies on
/// the operator >().
# define ELLE_OPERATOR_LTE(Type)                                        \
  public:                                                               \
  elle::Boolean                                                         \
  operator <=(Type const& other) const                                  \
  {                                                                     \
    return (!this->operator >(other));                                  \
  }

#endif
