#pragma once

#include <type_traits>

#include <boost/call_traits.hpp>
#include <boost/preprocessor/seq/cat.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/variadic/to_seq.hpp>

/// Define a private attribute.
/// This is equivalent to:
///
/// private:
///   <Type> _<Name>;
///
#define ELLE_ATTRIBUTE(Type, ...)                                   \
  ELLE_ATTRIBUTE_(Type, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))      \

#define ELLE_ATTRIBUTE_(Type, Args)                                 \
  private:                                                          \
  ELLE_ATTRIBUTE_PROPERTIES_PRE(BOOST_PP_SEQ_TAIL(Args))            \
  ELLE_ATTRIBUTE_PROPERTIES_PRETYPE(BOOST_PP_SEQ_TAIL(Args))        \
  ELLE_ATTRIBUTE_STRIP_PARENS(Type)                                 \
  BOOST_PP_CAT(_, BOOST_PP_SEQ_HEAD(Args));                         \

/*--.
| r |
`--*/

/// Define an accessor for attribute _Name.
///
/// This is equivalent to:
///
/// public:
///   <Type> [const&]
///   <Name>() const
///
#define ELLE_attribute_r(Type, Name, ...)                           \
  public:                                                           \
  ELLE_ATTRIBUTE_PROPERTIES_PREFUN(                                 \
    BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))                          \
  ::elle::_detail::attribute_r_t<ELLE_ATTRIBUTE_STRIP_PARENS(Type)> \
  Name() const                                                      \
  ELLE_ATTRIBUTE_PROPERTIES_POSTFUN(                                \
    BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))                          \

/// Define and implement an accessor for attribute _Name.
///
/// This is equivalent to:
///
/// public:
///   <Type> [const&]
///   <Name>() const
///   {
///     return this->_<Name>;
///   }
///
#define ELLE_attribute_R(Type, Name, ...)                           \
  ELLE_attribute_r(Type, Name, __VA_ARGS__)                         \
  {                                                                 \
    return this->BOOST_PP_CAT(_, Name);                             \
  }                                                                 \

/// Define the private attribute _Name and define its accessor.
///
/// This is equivalent to:
///
/// private:
///   <Type> _<Name>;
/// public:
///   <Type>[ const&]
///   name() const
///
#define ELLE_ATTRIBUTE_r(Type, Name, ...)                           \
  ELLE_ATTRIBUTE(Type, Name, __VA_ARGS__)                           \
  ELLE_attribute_r(Type, Name, __VA_ARGS__)                         \

/// Define the private attribute _Name and define and implement its accessor.
///
/// This is equivalent to:
///
/// private:
///   <Type> _<Name>;
/// public:
///   <Type>[ const&]
///   name() const
///   {
///     return this->_<Name>;
///   }
///
#define ELLE_ATTRIBUTE_R(Type, Name, ...)                           \
  ELLE_ATTRIBUTE(Type, Name, __VA_ARGS__)                           \
  ELLE_attribute_R(Type, Name, __VA_ARGS__)                         \

/*--.
| w |
`--*/

/// Define a setter for attribute _Name.
///
/// This is equivalent to:
///
/// public:
///   void
///   <Name>(<Type>[ const&] val)
///
#define ELLE_attribute_w(Type, Name, ...)                           \
  public:                                                           \
  ELLE_ATTRIBUTE_PROPERTIES_PREFUN(                                 \
    BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))                          \
  void                                                              \
  Name(typename boost::call_traits                                  \
         <ELLE_ATTRIBUTE_STRIP_PARENS(Type)>::param_type name)      \
  ELLE_ATTRIBUTE_PROPERTIES_POSTFUN(                                \
    BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))                          \

/// Define and implement a setter for attribute _Name.
///
/// This is equivalent to:
///
/// public:
///   void
///   <Name>(<Type>[ const&] val)
///   {
///     this->_<Name> = val;
///   }
///
#define ELLE_attribute_W(Type, Name, ...)                           \
  ELLE_attribute_w(Type, Name, __VA_ARGS__)                         \
  {                                                                 \
    this->BOOST_PP_CAT(_, Name) = name;                             \
  }                                                                 \

/// Define a private attribute _Name and define its setter.
///
/// This is equivalent to:
///
/// private:
///   <Type> _<Name>;
/// public:
///   void
///   <Name>(<Type>[ const&] val)
///
#define ELLE_ATTRIBUTE_w(Type, Name, ...)                           \
  ELLE_ATTRIBUTE(Type, Name, __VA_ARGS__)                           \
  ELLE_attribute_w(Type, Name, __VA_ARGS__)                         \

/// Define a private attribute and define and implement its setter.
///
/// This is equivalent to:
///
/// private:
///   <Type> _<Name>;
/// public:
///   void
///   <Name>(<Type>[ const&] val)
///   {
///     this->_<Name> = val;
///   }
///
#define ELLE_ATTRIBUTE_W(Type, Name, ...)                           \
  ELLE_ATTRIBUTE(Type, Name, __VA_ARGS__)                           \
  ELLE_attribute_W(Type, Name, __VA_ARGS__)                         \

/*--.
| x |
`--*/

/// Define a mutating accessor for attribute _Name.
///
/// This is equivalent to:
///
/// public:
///   <Type>&
///   <Name>()
#define ELLE_attribute_x(Type, Name, ...)                           \
  public:                                                           \
  ELLE_ATTRIBUTE_PROPERTIES_PREFUN(                                 \
    BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))                          \
  typename boost::call_traits                                       \
  <ELLE_ATTRIBUTE_STRIP_PARENS(Type)>::reference                    \
  Name()                                                            \
  ELLE_ATTRIBUTE_PROPERTIES_POSTFUN(                                \
    BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))                          \

/// Define and implement a mutating accessor for attribute _Name.
///
/// This is equivalent to:
///
/// {
///   return this->_<Name>;
/// }
#define ELLE_attribute_X(Type, Name, ...)                           \
  ELLE_attribute_x(Type, Name, __VA_ARGS__)                         \
  {                                                                 \
    return this->BOOST_PP_CAT(_, Name);                             \
  }                                                                 \

/// Define a private attribute _Name and define its mutating accessor.
///
/// This is equivalent to:
///
/// private:
///   <Type> _<Name>;
/// public:
///   <Type>&
///   <Name>()
///
#define ELLE_ATTRIBUTE_x(Type, Name, ...)                           \
  ELLE_ATTRIBUTE(Type, Name, __VA_ARGS__)                           \
  ELLE_attribute_x(Type, Name, __VA_ARGS__)                         \

/// Define a private attribute Name and define and implement its mutating
/// accessor.
///
/// This is equivalent to:
///
/// private:
///   <Type> _<Name>;
/// public:
///   <Type>&
///   <Name>()
///   {
///     return this->_<Name>;
///   }
#define ELLE_ATTRIBUTE_X(Type, Name, ...)                           \
  ELLE_ATTRIBUTE(Type, Name, __VA_ARGS__)                           \
  ELLE_attribute_X(Type, Name, __VA_ARGS__)                         \

/*---.
| rw |
`---*/

/// Define both accessor and setter for attribute _Name.
#define ELLE_attribute_rw(Type, Name, ...)                          \
  ELLE_attribute_r(Type, Name, __VA_ARGS__);                        \
  ELLE_attribute_w(Type, Name, __VA_ARGS__);                        \

/// Define a private attribute _Name and both its accessor and setter.
#define ELLE_ATTRIBUTE_rw(Type, Name, ...)                          \
  ELLE_ATTRIBUTE(Type, Name, __VA_ARGS__)                           \
  ELLE_attribute_rw(Type, Name, __VA_ARGS__)                        \

/// Define and implement accessor and define setter for attribute _Name.
#define ELLE_attribute_Rw(Type, Name, ...)                          \
  ELLE_attribute_R(Type, Name, __VA_ARGS__)                         \
  ELLE_attribute_w(Type, Name, __VA_ARGS__)                         \

/// Define a private attribute _Name, define and implement its accessor and
/// define its setter.
#define ELLE_ATTRIBUTE_Rw(Type, Name, ...)                          \
  ELLE_ATTRIBUTE(Type, Name, __VA_ARGS__)                           \
  ELLE_attribute_Rw(Type, Name, __VA_ARGS__)                        \

/// Define accessor and define and implement setter for attribute _Name.
#define ELLE_attribute_rW(Type, Name, ...)                          \
  ELLE_attribute_r(Type, Name, __VA_ARGS__);                        \
  ELLE_attribute_W(Type, Name, __VA_ARGS__)                         \

/// Define a private attribute _Name, define its accessor and define and
/// implement its setter.
#define ELLE_ATTRIBUTE_rW(Type, Name, ...)                          \
  ELLE_ATTRIBUTE(Type, Name, __VA_ARGS__)                           \
  ELLE_attribute_rW(Type, Name, __VA_ARGS__)                        \

/// Define and implement both accessor and setter for attribute _Name.
#define ELLE_attribute_RW(Type, Name, ...)                          \
  ELLE_attribute_R(Type, Name, __VA_ARGS__)                         \
  ELLE_attribute_W(Type, Name, __VA_ARGS__)                         \

/// Define a private attribute _Name and define and implement both its accessor
/// and setter.
#define ELLE_ATTRIBUTE_RW(Type, Name, ...)                          \
  ELLE_ATTRIBUTE(Type, Name, __VA_ARGS__)                           \
  ELLE_attribute_RW(Type, Name, __VA_ARGS__)                        \

/*---.
| rx |
`---*/

/// Define both accessor and mutable accessor for attribute _Name.
#define ELLE_attribute_rx(Type, Name, ...)                          \
  ELLE_attribute_r(Type, Name, __VA_ARGS__);                        \
  ELLE_attribute_x(Type, Name, __VA_ARGS__);                        \

/// Define a private attribute _Name both its accessor and mutable accessor.
#define ELLE_ATTRIBUTE_rx(Type, Name, ...)                          \
  ELLE_ATTRIBUTE(Type, Name, __VA_ARGS__)                           \
  ELLE_attribute_rx(Type, Name, __VA_ARGS__)                        \

/// Define and implement accessor and define mutable accessor for attribute
/// _Name.
#define ELLE_attribute_Rx(Type, Name, ...)                          \
  ELLE_attribute_R(Type, Name, __VA_ARGS__)                         \
  ELLE_attribute_x(Type, Name, __VA_ARGS__)                         \

/// Define a private attribute _Name, define and implement its accessor
/// and define its mutable accessor.
#define ELLE_ATTRIBUTE_Rx(Type, Name, ...)                          \
  ELLE_ATTRIBUTE(Type, Name, __VA_ARGS__)                           \
  ELLE_attribute_Rx(Type, Name, __VA_ARGS__)                        \

/// Define accessor and define and implement mutable accessor for attribute
/// _Name.
#define ELLE_attribute_rX(Type, Name, ...)                          \
  ELLE_attribute_r(Type, Name, __VA_ARGS__);                        \
  ELLE_attribute_X(Type, Name, __VA_ARGS__)                         \

/// Define a private attribute _Name, define its accessor and define and
/// implement its mutable accessor.
#define ELLE_ATTRIBUTE_rX(Type, Name, ...)                          \
  ELLE_ATTRIBUTE(Type, Name, __VA_ARGS__)                           \
  ELLE_attribute_rX(Type, Name, __VA_ARGS__)                        \

/// Define and implement both mutable accessor and mutable accessor for attribute
/// _Name.
#define ELLE_attribute_RX(Type, Name, ...)                          \
  ELLE_attribute_R(Type, Name, __VA_ARGS__)                         \
  ELLE_attribute_X(Type, Name, __VA_ARGS__)                         \

/// Define a private attribute _Name and define and implement both its
/// accessor and mutable accessor.
#define ELLE_ATTRIBUTE_RX(Type, Name, ...)                          \
  ELLE_ATTRIBUTE(Type, Name, __VA_ARGS__)                           \
  ELLE_attribute_RX(Type, Name, __VA_ARGS__)                        \

/*---.
| wx |
`---*/

/// Define both setter and mutable accessor for attribute _Name.
#define ELLE_attribute_wx(Type, Name, ...)                          \
  ELLE_attribute_w(Type, Name, __VA_ARGS__);                        \
  ELLE_attribute_x(Type, Name, __VA_ARGS__);                        \

/// Define a private attribute _Name both its setter and mutable accessor.
#define ELLE_ATTRIBUTE_wx(Type, Name, ...)                          \
  ELLE_ATTRIBUTE(Type, Name, __VA_ARGS__)                           \
  ELLE_attribute_wx(Type, Name, __VA_ARGS__)                        \

/// Define and implement setter and define setter for attribute _Name.
#define ELLE_attribute_Wx(Type, Name, ...)                          \
  ELLE_attribute_w(Type, Name, __VA_ARGS__)                         \
  ELLE_attribute_x(Type, Name, __VA_ARGS__)                         \

/// Define a private attribute _Name, define and implement its setter
/// and define its setter.
#define ELLE_ATTRIBUTE_Wx(Type, Name, ...)                          \
  ELLE_ATTRIBUTE(Type, Name, __VA_ARGS__)                           \
  ELLE_attribute_wx(Type, Name, __VA_ARGS__)                        \

/// Define setter and define and implement setter for attribute _Name.
#define ELLE_attribute_wX(Type, Name, ...)                          \
  ELLE_attribute_w(Type, Name, __VA_ARGS__);                        \
  ELLE_attribute_X(Type, Name, __VA_ARGS__)                         \

/// Define a private attribute _Name, define its setter and define and
/// implement its setter.
#define ELLE_ATTRIBUTE_wX(Type, Name, ...)                          \
  ELLE_ATTRIBUTE(Type, Name, __VA_ARGS__)                           \
  ELLE_attribute_wX(Type, Name, __VA_ARGS__)                        \

/// Define and implement both setter and setter for attribute _Name.
#define ELLE_attribute_WX(Type, Name, ...)                          \
  ELLE_attribute_w(Type, Name, __VA_ARGS__)                         \
  ELLE_attribute_X(Type, Name, __VA_ARGS__)                         \

/// Define a private attribute _Name and define and implement both its mutable
/// accessor and setter.
#define ELLE_ATTRIBUTE_WX(Type, Name, ...)                          \
  ELLE_ATTRIBUTE(Type, Name, __VA_ARGS__)                           \
  ELLE_attribute_wX(Type, Name, __VA_ARGS__)                        \

/*----.
| rwx |
`----*/

/// Define accessor, setter and mutable accessor for attribute _Name.
#define ELLE_attribute_rwx(Type, Name, ...)                         \
  ELLE_attribute_r(Type, Name, __VA_ARGS__);                        \
  ELLE_attribute_w(Type, Name, __VA_ARGS__);                        \
  ELLE_attribute_x(Type, Name, __VA_ARGS__);                        \

/// Define a private attribute _Name and its accessor, setter and mutable
/// accessor.
#define ELLE_ATTRIBUTE_rwx(Type, Name, ...)                         \
  ELLE_ATTRIBUTE(Type, Name, __VA_ARGS__)                           \
  ELLE_attribute_rwx(Type, Name, __VA_ARGS__)                       \

/// Define and implement accessor and define setter and mutable accessor for
/// attribute _Name.
#define ELLE_attribute_Rwx(Type, Name, ...)                         \
  ELLE_attribute_R(Type, Name, __VA_ARGS__)                         \
  ELLE_attribute_w(Type, Name, __VA_ARGS__)                         \
  ELLE_attribute_x(Type, Name, __VA_ARGS__)                         \

/// Define a private attribute _Name, define and implement its accessor and
/// define its setter and mutable accessor.
#define ELLE_ATTRIBUTE_Rwx(Type, Name, ...)                         \
  ELLE_ATTRIBUTE(Type, Name, __VA_ARGS__)                           \
  ELLE_attribute_Rwx(Type, Name, __VA_ARGS__)                       \

/// Define and implement setter and define accessor and mutable accessor for
/// attribute _Name.
#define ELLE_attribute_rWx(Type, Name, ...)                         \
  ELLE_attribute_r(Type, Name, __VA_ARGS__);                        \
  ELLE_attribute_x(Type, Name, __VA_ARGS__);                        \
  ELLE_attribute_W(Type, Name, __VA_ARGS__)                         \

/// Define a private attribute _Name, define and implement its setter and
/// define its accessor and mutable accessor.
#define ELLE_ATTRIBUTE_rWx(Type, Name, ...)                         \
  ELLE_ATTRIBUTE(Type, Name, __VA_ARGS__)                           \
  ELLE_attribute_rWx(Type, Name, __VA_ARGS__)                       \

/// Define and implement setter and accessor and define mutable accessor for
/// attribute _Name.
#define ELLE_attribute_RWx(Type, Name, ...)                         \
  ELLE_attribute_R(Type, Name, __VA_ARGS__)                         \
  ELLE_attribute_W(Type, Name, __VA_ARGS__)                         \
  ELLE_attribute_x(Type, Name, __VA_ARGS__)                         \

/// Define a private attribute _Name, define and implement its accessor and
/// setter and define its mutable accessor.
#define ELLE_ATTRIBUTE_RWx(Type, Name, ...)                         \
  ELLE_ATTRIBUTE(Type, Name, __VA_ARGS__)                           \
  ELLE_attribute_RWx(Type, Name, __VA_ARGS__)                       \

/// Define and implement mutable accessor and define accessor and setter for
/// attribute _Name.
#define ELLE_attribute_rwX(Type, Name, ...)                         \
  ELLE_attribute_r(Type, Name, __VA_ARGS__);                        \
  ELLE_attribute_w(Type, Name, __VA_ARGS__);                        \
  ELLE_attribute_X(Type, Name, __VA_ARGS__);                        \

/// Define a private attribute _Name, define and implement its mutable accessor
/// and define its accessor and setter.
#define ELLE_ATTRIBUTE_rwX(Type, Name, ...)                         \
  ELLE_ATTRIBUTE(Type, Name, __VA_ARGS__)                           \
  ELLE_attribute_rwX(Type, Name, __VA_ARGS__)                       \

/// Define and implement accessor and mutable accessor and define setter for
/// attribute _Name.
#define ELLE_attribute_RwX(Type, Name, ...)                         \
  ELLE_attribute_R(Type, Name, __VA_ARGS__)                         \
  ELLE_attribute_w(Type, Name, __VA_ARGS__)                         \
  ELLE_attribute_X(Type, Name, __VA_ARGS__)                         \

/// Define a private attribute _Name, define and implement its accessor and
/// mutable accessor and define its setter.
#define ELLE_ATTRIBUTE_RwX(Type, Name, ...)                         \
  ELLE_ATTRIBUTE(Type, Name, __VA_ARGS__)                           \
  ELLE_attribute_RwX(Type, Name, __VA_ARGS__)                       \

/// Define and implement setter and mutable accessor and define accessor for
/// attribute _Name.
#define ELLE_attribute_rWX(Type, Name, ...)                         \
  ELLE_attribute_r(Type, Name, __VA_ARGS__);                        \
  ELLE_attribute_W(Type, Name, __VA_ARGS__)                         \
  ELLE_attribute_X(Type, Name, __VA_ARGS__);                        \

/// Define a private attribute _Name, define and implement its setter and
/// mutable accessor and define its setter.
#define ELLE_ATTRIBUTE_rWX(Type, Name, ...)                         \
  ELLE_ATTRIBUTE(Type, Name, __VA_ARGS__)                           \
  ELLE_attribute_rWX(Type, Name, __VA_ARGS__)                       \

/// Define and implement accessor, setter and mutable accessor for attribute
/// _Name.
#define ELLE_attribute_RWX(Type, Name, ...)                         \
  ELLE_attribute_R(Type, Name, __VA_ARGS__)                         \
  ELLE_attribute_W(Type, Name, __VA_ARGS__)                         \
  ELLE_attribute_X(Type, Name, __VA_ARGS__)                         \

/// Define a private attribute _Name, define and implement its accessor, setter
/// and mutable accessor.
#define ELLE_ATTRIBUTE_RWX(Type, Name, ...)     \
  ELLE_ATTRIBUTE(Type, Name, __VA_ARGS__)                           \
  ELLE_attribute_RWX(Type, Name, __VA_ARGS__)                       \

/*------------.
| Thread safe |
`------------*/

/// Define and implement a thread safe accessor returning the attribute.
#define ELLE_ATTRIBUTE_R_TS_ACCESSOR(Type, Name)                    \
  public:                                                           \
  ELLE_ATTRIBUTE_STRIP_PARENS(Type)                                 \
  Name() const                                                      \
  {                                                                 \
    std::unique_lock<elle::threading::read_mutex> lock              \
      (this->BOOST_PP_CAT(BOOST_PP_CAT(_, Name), _mutex));          \
    return this->BOOST_PP_CAT(_, Name);                             \
  }

/// Define and implement a thread safe accessor enabling one to set the
/// attribute.
#define ELLE_ATTRIBUTE_W_TS_ACCESSOR(Type, Name)                    \
  ELLE_attribute_w(Type, Name)                                      \
  {                                                                 \
    std::unique_lock<elle::threading::write_mutex> lock             \
      (this->BOOST_PP_CAT(BOOST_PP_CAT(_, Name), _mutex));          \
    this->BOOST_PP_CAT(_, Name) = name;                             \
  }                                                                 \

#define ELLE_ATTRIBUTE_RW_TS_ACCESSORS(Type, Name)                  \
  ELLE_ATTRIBUTE_R_TS_ACCESSOR(Type, Name)                          \
  ELLE_ATTRIBUTE_W_TS_ACCESSOR(Type, Name)                          \

#define ELLE_ATTRIBUTE_RW_TS(Type, Name, ...)                       \
  ELLE_ATTRIBUTE(Type, Name, __VA_ARGS__);                          \
  mutable ::elle::threading::rw_mutex                               \
  BOOST_PP_CAT(BOOST_PP_CAT(_, Name), _mutex);                      \
  ELLE_ATTRIBUTE_RW_TS_ACCESSORS(Type, Name)                        \

/*-----------.
| Properties |
`-----------*/

#define ELLE_ATTRIBUTE_PROPERTY__PRE
#define ELLE_ATTRIBUTE_PROPERTY_mutable_PRE
#define ELLE_ATTRIBUTE_PROPERTY_override_PRE
#define ELLE_ATTRIBUTE_PROPERTY_protected_PRE protected:
#define ELLE_ATTRIBUTE_PROPERTY_public_PRE public:
#define ELLE_ATTRIBUTE_PROPERTY_static_PRE
#define ELLE_ATTRIBUTE_PROPERTY_virtual_PRE

#define ELLE_ATTRIBUTE_PROPERTY__PRETYPE
#define ELLE_ATTRIBUTE_PROPERTY_mutable_PRETYPE mutable
#define ELLE_ATTRIBUTE_PROPERTY_override_PRETYPE
#define ELLE_ATTRIBUTE_PROPERTY_protected_PRETYPE
#define ELLE_ATTRIBUTE_PROPERTY_public_PRETYPE
#define ELLE_ATTRIBUTE_PROPERTY_static_PRETYPE static
#define ELLE_ATTRIBUTE_PROPERTY_virtual_PRETYPE

#define ELLE_ATTRIBUTE_PROPERTY__PREFUN
#define ELLE_ATTRIBUTE_PROPERTY_mutable_PREFUN
#define ELLE_ATTRIBUTE_PROPERTY_override_PREFUN
#define ELLE_ATTRIBUTE_PROPERTY_protected_PREFUN
#define ELLE_ATTRIBUTE_PROPERTY_public_PREFUN
#define ELLE_ATTRIBUTE_PROPERTY_static_PREFUN
#define ELLE_ATTRIBUTE_PROPERTY_virtual_PREFUN virtual


#define ELLE_ATTRIBUTE_PROPERTY__POSTFUN
#define ELLE_ATTRIBUTE_PROPERTY_mutable_POSTFUN
#define ELLE_ATTRIBUTE_PROPERTY_override_POSTFUN override
#define ELLE_ATTRIBUTE_PROPERTY_protected_POSTFUN
#define ELLE_ATTRIBUTE_PROPERTY_public_POSTFUN
#define ELLE_ATTRIBUTE_PROPERTY_static_POSTFUN
#define ELLE_ATTRIBUTE_PROPERTY_virtual_POSTFUN

/*--------.
| Helpers |
`--------*/

#define ELLE_ATTRIBUTE_APPLY(macro, args) ELLE_ATTRIBUTE_APPLY_I(macro, args)
#define ELLE_ATTRIBUTE_APPLY_I(macro, args) macro args
#define ELLE_ATTRIBUTE_STRIP_PARENS(x) ELLE_ATTRIBUTE_EVAL((ELLE_ATTRIBUTE_STRIP_PARENS_I x), x)
#define ELLE_ATTRIBUTE_STRIP_PARENS_I(...) 1,1
#define ELLE_ATTRIBUTE_EVAL(test, x) ELLE_ATTRIBUTE_EVAL_I(test, x)
#define ELLE_ATTRIBUTE_EVAL_I(test, x) ELLE_ATTRIBUTE_MAYBE_STRIP_PARENS(ELLE_ATTRIBUTE_TEST_ARITY test, x)
#define ELLE_ATTRIBUTE_TEST_ARITY(...) ELLE_ATTRIBUTE_APPLY(ELLE_ATTRIBUTE_TEST_ARITY_I, (__VA_ARGS__, 2, 1))
#define ELLE_ATTRIBUTE_TEST_ARITY_I(a,b,c,...) c
#define ELLE_ATTRIBUTE_MAYBE_STRIP_PARENS(cond, x) ELLE_ATTRIBUTE_MAYBE_STRIP_PARENS_I(cond, x)
#define ELLE_ATTRIBUTE_MAYBE_STRIP_PARENS_I(cond, x) BOOST_PP_CAT(ELLE_ATTRIBUTE_MAYBE_STRIP_PARENS_, cond)(x)
#define ELLE_ATTRIBUTE_MAYBE_STRIP_PARENS_1(x) x
#define ELLE_ATTRIBUTE_MAYBE_STRIP_PARENS_2(x) ELLE_ATTRIBUTE_APPLY(ELLE_ATTRIBUTE_MAYBE_STRIP_PARENS_2_I, x)
#define ELLE_ATTRIBUTE_MAYBE_STRIP_PARENS_2_I(...) __VA_ARGS__
#define ELLE_ATTRIBUTE_PROPERTIES_PRE(Properties)                      \
  BOOST_PP_SEQ_FOR_EACH(ELLE_ATTRIBUTE_PROPERTY, _PRE, Properties)
#define ELLE_ATTRIBUTE_PROPERTIES_PRETYPE(Properties)                  \
  BOOST_PP_SEQ_FOR_EACH(ELLE_ATTRIBUTE_PROPERTY, _PRETYPE, Properties)
#define ELLE_ATTRIBUTE_PROPERTIES_PREFUN(Properties)                   \
  BOOST_PP_SEQ_FOR_EACH(ELLE_ATTRIBUTE_PROPERTY, _PREFUN, Properties)
#define ELLE_ATTRIBUTE_PROPERTIES_POSTFUN(Properties)                   \
  BOOST_PP_SEQ_FOR_EACH(ELLE_ATTRIBUTE_PROPERTY, _POSTFUN, Properties)
#define ELLE_ATTRIBUTE_PROPERTY(R, Data, Elem)                         \
  BOOST_PP_CAT(BOOST_PP_CAT(ELLE_ATTRIBUTE_PROPERTY_, Elem), Data)

namespace elle
{
  namespace _detail
  {
    template <typename T>
    struct attribute_r_type
    {
      constexpr static auto plain
        = (std::is_fundamental<T>{}
           || std::is_enum<T>{}
           || std::is_pointer<T>{});

      using type = std::conditional_t<plain, T, T const&>;
    };

    template <typename T>
    using attribute_r_t = typename attribute_r_type<T>::type;
  }
}
