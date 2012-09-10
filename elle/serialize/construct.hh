#ifndef  ELLE_SERIALIZE_CONSTRUCT_HH
# define ELLE_SERIALIZE_CONSTRUCT_HH

//
// Add a deserialization constructor to a class __T. Following parameters are
// respectively the direct base classes and the attributes that are
// "archive-constructible". You'll have to define a special NoInit constructor,
// that ensure that object is not initialized, yet destructible.  In most case
// you won't have to do anything, but remember that the object could be deleted
// before being completely deserialized.
//
// Example 1: Simple class
// --------------------------------------------
// struct A
// {
//   ELLE_SERIALIZE_CONSTRUCT(A)
//   {
//      // Called before deserialization process. Makes the object ready to
//      // be filled by deserialization (usually, you'll just do nothing).
//   }
// };
// --------------------------------------------
//
// Example 2: Inheritance
// --------------------------------------------
// struct B: public A
// {
//    ELLE_SERIALIZE_CONSTRUCT(B, A) {}
// };
// --------------------------------------------
//
// Example 3: Aggregation
// --------------------------------------------
// struct C
// {
//    int i;
//    B b;
//    ELLE_SERIALIZE_CONSTRUCT(C, b) {}
// };
// --------------------------------------------
//
// Example 4: Separated implementation
// --------------------------------------------
// struct D
// {
//   ELLE_SERIALIZE_CONSTRUCT_DECL(D);
// }
// ELLE_SERIALIZE_CONSTRUCT_DEF(D)
// {
//    // here we go
// }
// --------------------------------------------
//
// Example 5: Do not forgot pointers !
// --------------------------------------------
// struct E
// {
// private:
//    A* _ptr;
//
// public:
//    E(): _ptr{nullptr} {} // normal construction
//    ~E() { delete _ptr; } // _ptr should point to a valid address
//    ELLE_SERIALIZE_CONSTRUCT(E)
//    {
//      _ptr = nullptr; // in case of errors, the destructor will not try to
//                      // delete a random address (_ptr not initialized).
//    }
// };
// --------------------------------------------
# define ELLE_SERIALIZE_CONSTRUCT(__T, ...)                                   \
  ELLE_SERIALIZE_CONSTRUCT_DECL(__T)                                          \
    __ESC_INITIALIZATION_LIST(__T, ##__VA_ARGS__)                             \
/**/

///
/// Make the type __T "archive-constructible" and defines the
/// pre-deserialization constructor.
///
/// XXX: Should not work with templated types, use ELLE_SERIALIZE_CONSTRUCT
///      instead.
///
# define ELLE_SERIALIZE_CONSTRUCT_DECL(__T)                                   \
  template <typename Archive>                                                 \
  explicit                                                                    \
  __T(Archive& archive):                                                      \
    __T{elle::serialize::no_init}                                             \
  {                                                                           \
    archive & *this;                                                          \
  }                                                                           \
  explicit                                                                    \
  __T(elle::serialize::NoInit)                                                \
/**/

/// Define implementation of the pre-deserialization constructor.
# define ELLE_SERIALIZE_CONSTRUCT_DEF(__T, ...)                               \
  __T::__T(elle::serialize::NoInit)                                           \
    __ESC_INITIALIZATION_LIST(__T, ##__VA_ARGS__)                             \
/**/

//
//- Internal types ------------------------------------------------------------
//

namespace elle
{
  namespace serialize
  {

    enum NoInit { no_init };

  }
}

//
//- Internal macros -----------------------------------------------------------
//

# include <boost/preprocessor/arithmetic/dec.hpp>
# include <boost/preprocessor/control/if.hpp>
# include <boost/preprocessor/facilities/empty.hpp>
# include <boost/preprocessor/punctuation/comma.hpp>
# include <boost/preprocessor/seq/for_each_i.hpp>
# include <boost/preprocessor/variadic/size.hpp>
# include <boost/preprocessor/variadic/to_seq.hpp>

# define __ESC_INITIALIZATION_LIST_REPEAT(__r, __data, __i, __elem)           \
  BOOST_PP_IF(__i, BOOST_PP_COMMA, :)                                         \
  __elem{elle::serialize::no_init}                                            \
/**/

# define __ESC_INITIALIZATION_LIST(__T, ...)                                  \
  BOOST_PP_IF(                                                                \
    BOOST_PP_DEC(BOOST_PP_VARIADIC_SIZE(__T, ##__VA_ARGS__)),                 \
    BOOST_PP_SEQ_FOR_EACH_I(                                                  \
      __ESC_INITIALIZATION_LIST_REPEAT,                                       \
      _,                                                                      \
      BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)                                   \
    ),                                                                        \
    BOOST_PP_EMPTY()                                                          \
  )                                                                           \
/**/

#endif
