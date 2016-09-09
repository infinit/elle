#ifndef ELLE_ATTRIBUTE_HH
# define ELLE_ATTRIBUTE_HH

# include <type_traits>

# include <boost/call_traits.hpp>
# include <boost/preprocessor/seq/cat.hpp>
# include <boost/preprocessor/seq/for_each.hpp>
# include <boost/preprocessor/variadic/to_seq.hpp>

namespace elle
{
  namespace _detail
  {
    template <typename T>
    struct attribute_r_type
    {
      typedef typename std::conditional<
        std::is_fundamental<T>::value ||
        std::is_enum<T>::value ||
        std::is_pointer<T>::value,
        T, T const&>::type type;
    };
  }
}

# define ELLE_ATTRIBUTE_APPLY(macro, args) ELLE_ATTRIBUTE_APPLY_I(macro, args)
# define ELLE_ATTRIBUTE_APPLY_I(macro, args) macro args
# define ELLE_ATTRIBUTE_STRIP_PARENS(x) ELLE_ATTRIBUTE_EVAL((ELLE_ATTRIBUTE_STRIP_PARENS_I x), x)
# define ELLE_ATTRIBUTE_STRIP_PARENS_I(...) 1,1
# define ELLE_ATTRIBUTE_EVAL(test, x) ELLE_ATTRIBUTE_EVAL_I(test, x)
# define ELLE_ATTRIBUTE_EVAL_I(test, x) ELLE_ATTRIBUTE_MAYBE_STRIP_PARENS(ELLE_ATTRIBUTE_TEST_ARITY test, x)
# define ELLE_ATTRIBUTE_TEST_ARITY(...) ELLE_ATTRIBUTE_APPLY(ELLE_ATTRIBUTE_TEST_ARITY_I, (__VA_ARGS__, 2, 1))
# define ELLE_ATTRIBUTE_TEST_ARITY_I(a,b,c,...) c
# define ELLE_ATTRIBUTE_MAYBE_STRIP_PARENS(cond, x) ELLE_ATTRIBUTE_MAYBE_STRIP_PARENS_I(cond, x)
# define ELLE_ATTRIBUTE_MAYBE_STRIP_PARENS_I(cond, x) BOOST_PP_CAT(ELLE_ATTRIBUTE_MAYBE_STRIP_PARENS_, cond)(x)
# define ELLE_ATTRIBUTE_MAYBE_STRIP_PARENS_1(x) x
# define ELLE_ATTRIBUTE_MAYBE_STRIP_PARENS_2(x) ELLE_ATTRIBUTE_APPLY(ELLE_ATTRIBUTE_MAYBE_STRIP_PARENS_2_I, x)
# define ELLE_ATTRIBUTE_MAYBE_STRIP_PARENS_2_I(...) __VA_ARGS__
# define ELLE_ATTRIBUTE_PROPERTIES_PRE(Properties)              \
  BOOST_PP_SEQ_FOR_EACH(ELLE_ATTRIBUTE_PROPERTY, _PRE, Properties)
# define ELLE_ATTRIBUTE_PROPERTIES_PRETYPE(Properties)          \
  BOOST_PP_SEQ_FOR_EACH(ELLE_ATTRIBUTE_PROPERTY, _PRETYPE, Properties)
# define ELLE_ATTRIBUTE_PROPERTY(R, Data, Elem)                         \
  BOOST_PP_CAT(BOOST_PP_CAT(ELLE_ATTRIBUTE_PROPERTY_, Elem), Data)

# define ELLE_ATTRIBUTE_PROPERTY__PRE
# define ELLE_ATTRIBUTE_PROPERTY_mutable_PRE public:
# define ELLE_ATTRIBUTE_PROPERTY_protected_PRE protected:
# define ELLE_ATTRIBUTE_PROPERTY_public_PRE public:
# define ELLE_ATTRIBUTE_PROPERTY_static_PRE
# define ELLE_ATTRIBUTE_PROPERTY__PRETYPE
# define ELLE_ATTRIBUTE_PROPERTY_mutable_PRETYPE mutable
# define ELLE_ATTRIBUTE_PROPERTY_protected_PRETYPE
# define ELLE_ATTRIBUTE_PROPERTY_public_PRETYPE
# define ELLE_ATTRIBUTE_PROPERTY_static_PRETYPE static

/// Define a private attribute.
# define ELLE_ATTRIBUTE(Type, ...)                                      \
  ELLE_ATTRIBUTE_(Type, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))          \

# define ELLE_ATTRIBUTE_(Type, Args)                                    \
  private:                                                              \
  ELLE_ATTRIBUTE_PROPERTIES_PRE(BOOST_PP_SEQ_TAIL(Args))                \
  ELLE_ATTRIBUTE_PROPERTIES_PRETYPE(BOOST_PP_SEQ_TAIL(Args))            \
  ELLE_ATTRIBUTE_STRIP_PARENS(Type)                                     \
  BOOST_PP_CAT(_, BOOST_PP_SEQ_HEAD(Args));                             \

/// Define an accessor returning a const reference on the attribute.
# define ELLE_ATTRIBUTE_r_ACCESSOR(_type_, _name_)                      \
  public:                                                               \
  typename ::elle::_detail::attribute_r_type                            \
  <ELLE_ATTRIBUTE_STRIP_PARENS(_type_)>::type                           \
  _name_() const                                                        \

/// Define and implement an accessor returning a const reference
/// on the attribute.
# define ELLE_ATTRIBUTE_R_ACCESSOR(_type_, _name_)                      \
  ELLE_ATTRIBUTE_r_ACCESSOR(_type_, _name_)                             \
  {                                                                     \
    return (this->BOOST_PP_CAT(_, _name_));                             \
  }                                                                     \

/// Define and implement a thread safe accessor returning the attribute.
# define ELLE_ATTRIBUTE_R_TS_ACCESSOR(_type_, _name_)                   \
  public:                                                               \
  ELLE_ATTRIBUTE_STRIP_PARENS(_type_)                                   \
  _name_() const                                                        \
  {                                                                     \
    std::unique_lock<elle::threading::read_mutex> lock                  \
      (this->BOOST_PP_CAT(BOOST_PP_CAT(_, _name_), _mutex));            \
    return (this->BOOST_PP_CAT(_, _name_));                             \
  }

/// Define an accessor enabling one to set the attribute.
# define ELLE_ATTRIBUTE_w_ACCESSOR(_type_, _name_)                      \
  public:                                                               \
  void                                                                  \
  _name_(typename boost::call_traits                                    \
         <ELLE_ATTRIBUTE_STRIP_PARENS(_type_)>::param_type name)        \

/// Define and implement an accessor enabling one to set the attribute.
# define ELLE_ATTRIBUTE_W_ACCESSOR(_type_, _name_)                      \
  ELLE_ATTRIBUTE_w_ACCESSOR(_type_, _name_)                             \
  {                                                                     \
    this->BOOST_PP_CAT(_, _name_) = name;                               \
  }                                                                     \

/// Define and implement a thread safe accessor enabling one to set the
/// attribute.
# define ELLE_ATTRIBUTE_W_TS_ACCESSOR(_type_, _name_)                   \
  ELLE_ATTRIBUTE_w_ACCESSOR(_type_, _name_)                             \
  {                                                                     \
    std::unique_lock<elle::threading::write_mutex> lock                 \
      (this->BOOST_PP_CAT(BOOST_PP_CAT(_, _name_), _mutex));            \
    this->BOOST_PP_CAT(_, _name_) = name;                               \
  }                                                                     \

/// Define an accessor returning a reference on the attribute i.e
/// enabling one to potentially modify the attribute outside the class.
# define ELLE_ATTRIBUTE_x_ACCESSOR(_type_, _name_)                      \
  public:                                                               \
  typename boost::call_traits                                           \
  <ELLE_ATTRIBUTE_STRIP_PARENS(_type_)>::reference                      \
  _name_()                                                              \

/// Define and implement an accessor returning a reference on the attribute i.e
/// enabling one to potentially modify the attribute outside the class.
# define ELLE_ATTRIBUTE_X_ACCESSOR(_type_, _name_)                      \
  ELLE_ATTRIBUTE_x_ACCESSOR(_type_, _name_)                             \
  {                                                                     \
    return (this->BOOST_PP_CAT(_, _name_));                             \
  }                                                                     \

# define ELLE_ATTRIBUTE_RW_ACCESSORS(_type_, _name_)                    \
  ELLE_ATTRIBUTE_R_ACCESSOR(_type_, _name_)                             \
  ELLE_ATTRIBUTE_W_ACCESSOR(_type_, _name_)                             \

# define ELLE_ATTRIBUTE_RW_TS_ACCESSORS(_type_, _name_)                 \
  ELLE_ATTRIBUTE_R_TS_ACCESSOR(_type_, _name_)                          \
  ELLE_ATTRIBUTE_W_TS_ACCESSOR(_type_, _name_)                          \

# define ELLE_ATTRIBUTE_RX_ACCESSORS(_type_, _name_)                    \
  ELLE_ATTRIBUTE_R_ACCESSOR(_type_, _name_)                             \
  ELLE_ATTRIBUTE_X_ACCESSOR(_type_, _name_)                             \

# define ELLE_ATTRIBUTE_WX_ACCESSORS(_type_, _name_)                    \
  ELLE_ATTRIBUTE_W_ACCESSOR(_type_, _name_)                             \
  ELLE_ATTRIBUTE_X_ACCESSOR(_type_, _name_)                             \

# define ELLE_ATTRIBUTE_RWX_ACCESSORS(_type_, _name_)                   \
  ELLE_ATTRIBUTE_R_ACCESSOR(_type_, _name_)                             \
  ELLE_ATTRIBUTE_W_ACCESSOR(_type_, _name_)                             \
  ELLE_ATTRIBUTE_X_ACCESSOR(_type_, _name_)                             \

# define ELLE_ATTRIBUTE_RwX_ACCESSORS(_type_, _name_)                   \
  ELLE_ATTRIBUTE_R_ACCESSOR(_type_, _name_)                             \
  ELLE_ATTRIBUTE_w_ACCESSOR(_type_, _name_);                            \
  ELLE_ATTRIBUTE_X_ACCESSOR(_type_, _name_)                             \

# define ELLE_ATTRIBUTE_R(_type_, _name_, ...)                          \
  ELLE_ATTRIBUTE(_type_, _name_, __VA_ARGS__)                           \
  ELLE_ATTRIBUTE_R_ACCESSOR(_type_, _name_)                             \

# define ELLE_ATTRIBUTE_r(_type_, _name_, ...)                          \
  ELLE_ATTRIBUTE(_type_, _name_, __VA_ARGS__)                           \
  ELLE_attribute_r(_type_, _name_, __VA_ARGS__)                         \

# define ELLE_attribute_r(_type_, _name_, ...)                          \
  ELLE_ATTRIBUTE_r_ACCESSOR(_type_, _name_)                             \

# define ELLE_ATTRIBUTE_W(_type_, _name_, ...)                          \
  ELLE_ATTRIBUTE(_type_, _name_, __VA_ARGS__)                           \
  ELLE_ATTRIBUTE_W_ACCESSOR(_type_, _name_)                             \

# define ELLE_ATTRIBUTE_X(_type_, _name_, ...)                          \
  ELLE_ATTRIBUTE(_type_, _name_, __VA_ARGS__)                           \
  ELLE_ATTRIBUTE_X_ACCESSOR(_type_, _name_)                             \

# define ELLE_ATTRIBUTE_RW(_type_, _name_, ...)                         \
  ELLE_ATTRIBUTE(_type_, _name_, __VA_ARGS__)                           \
  ELLE_ATTRIBUTE_RW_ACCESSORS(_type_, _name_)                           \

# define ELLE_ATTRIBUTE_RW_TS(_type_, _name_, ...)                      \
  ELLE_ATTRIBUTE(_type_, _name_, __VA_ARGS__);                          \
  mutable ::elle::threading::rw_mutex                                   \
  BOOST_PP_CAT(BOOST_PP_CAT(_, _name_), _mutex);                        \
  ELLE_ATTRIBUTE_RW_TS_ACCESSORS(_type_, _name_)                        \

# define ELLE_ATTRIBUTE_Rw(_type_, _name_, ...)                         \
  ELLE_ATTRIBUTE(_type_, _name_, __VA_ARGS__)                           \
  ELLE_ATTRIBUTE_R_ACCESSOR(_type_, _name_)                             \
  ELLE_ATTRIBUTE_w_ACCESSOR(_type_, _name_)                             \

# define ELLE_attribute_rw(_type_, _name_, ...)                         \
  ELLE_ATTRIBUTE_r_ACCESSOR(_type_, _name_);                            \
  ELLE_ATTRIBUTE_w_ACCESSOR(_type_, _name_);                            \

# define ELLE_ATTRIBUTE_rw(_type_, _name_, ...)                         \
  ELLE_ATTRIBUTE(_type_, _name_, __VA_ARGS__)                           \
  ELLE_attribute_rw(_type_, _name_, __VA_ARGS__)                        \

# define ELLE_ATTRIBUTE_rx(_type_, _name_, ...)                         \
  ELLE_ATTRIBUTE(_type_, _name_, __VA_ARGS__)                           \
  ELLE_ATTRIBUTE_r_ACCESSOR(_type_, _name_);                            \
  ELLE_ATTRIBUTE_x_ACCESSOR(_type_, _name_);                            \

# define ELLE_ATTRIBUTE_RX(_type_, _name_, ...)                         \
  ELLE_ATTRIBUTE(_type_, _name_, __VA_ARGS__)                           \
  ELLE_ATTRIBUTE_RX_ACCESSORS(_type_, _name_)                           \

# define ELLE_ATTRIBUTE_WX(_type_, _name_, ...)                         \
  ELLE_ATTRIBUTE(_type_, _name_, __VA_ARGS__)                           \
  ELLE_ATTRIBUTE_WX_ACCESSORS(_type_, _name_)                           \

# define ELLE_ATTRIBUTE_RWX(_type_, _name_, ...)                        \
  ELLE_ATTRIBUTE(_type_, _name_, __VA_ARGS__)                           \
  ELLE_ATTRIBUTE_RWX_ACCESSORS(_type_, _name_)                          \

# define ELLE_ATTRIBUTE_RwX(_type_, _name_, ...)                        \
  ELLE_ATTRIBUTE(_type_, _name_, __VA_ARGS__)                           \
  ELLE_ATTRIBUTE_RwX_ACCESSORS(_type_, _name_)                          \

#endif
