#pragma once

#include <boost/preprocessor/seq/cat.hpp>

/// Make it easy to define an operator =() method. However, one must
/// take into account that this implementation relies on the presence
/// of a copy constructor which may therefore need to be implemented
/// should the class contain pointers for example.
///
/// !WARNING! Don't use this unless you know exactly what you're doing.
#define ELLE_OPERATOR_ASSIGNMENT(_type_)                                \
  public:                                                               \
  _type_&                                                               \
  operator =(_type_ const& that)                                        \
  {                                                                     \
    if (this != &that)                                                  \
    {                                                                   \
      this->~_type_();                                                  \
      new (this) _type_(that);                                          \
    }                                                                   \
    return *this;                                                       \
  }

/// Produce a deleted operator =() so as to make sure one cannot
/// assign such a type.
#ifdef _MSC_VER
# define ELLE_OPERATOR_NO_ASSIGNMENT(_type_)                            \
  private:                                                              \
  _type_&                                                               \
  operator =(_type_ const&);                                            \
  _type_&                                                               \
  operator =(_type_&&);
#else
# define ELLE_OPERATOR_NO_ASSIGNMENT(_type_)                            \
  public:                                                               \
  _type_&                                                               \
  operator =(_type_ const&) = delete;                                   \
  public:                                                               \
  _type_&                                                               \
  operator =(_type_&&) = delete;
#endif

/// Generate the operator ==() method. Note that this method relies on
/// the operator <().
#define ELLE_OPERATOR_EQ(_type_)                                        \
  public:                                                               \
  bool                                                                  \
  operator ==(_type_ const& that) const                                 \
  {                                                                     \
    return !(*this < that || that < *this);                             \
  }

/// Generate the operator !=() method. Note that this method relies on
/// the operator ==().
#define ELLE_OPERATOR_NEQ(_type_)                                       \
  public:                                                               \
  bool                                                                  \
  operator !=(_type_ const& that) const                                 \
  {                                                                     \
    return !(*this == that);                                            \
  }

/// Generate the operator >() method. Note that this method relies on
/// the operator <().
#define ELLE_OPERATOR_GT(_type_)                                        \
  public:                                                               \
  bool                                                                  \
  operator >(_type_ const& that) const                                  \
  {                                                                     \
    return that < *this;                                                \
  }

/// Generate the operator >=() method. Note that this method relies on
/// the operator <().
#define ELLE_OPERATOR_GTE(_type_)                                       \
  public:                                                               \
  bool                                                                  \
  operator >=(_type_ const& that) const                                 \
  {                                                                     \
    return !(*this < that);                                             \
  }

/// Generate the operator <() method. Note that this method relies on
/// the operator >=().
#define ELLE_OPERATOR_LT(_type_)                                        \
  public:                                                               \
  bool                                                                  \
  operator <(_type_ const& that) const                                  \
  {                                                                     \
    return !(*this >= that);                                            \
  }

/// Generate the operator <=() method. Note that this method relies on
/// the operator <().
#define ELLE_OPERATOR_LTE(_type_)                                       \
  public:                                                               \
  bool                                                                  \
  operator <=(_type_ const& that) const                                 \
  {                                                                     \
    return !(that < *this);                                             \
  }
