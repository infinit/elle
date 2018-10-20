#pragma once

#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/variadic/to_seq.hpp>

#define ELLE_ENUM(Name, ...)                                            \
  enum class Name                                                       \
  {                                                                     \
    BOOST_PP_SEQ_FOR_EACH(                                              \
      ELLE_ENUM_VALUE, , BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))         \
  };                                                                    \
                                                                        \
  inline                                                                \
  std::ostream&                                                         \
  operator <<(std::ostream& s, Name v)                                  \
  {                                                                     \
    switch (v)                                                          \
    {                                                                   \
      BOOST_PP_SEQ_FOR_EACH(                                            \
        ELLE_ENUM_PRINT, Name, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))   \
      default:                                                          \
        /* Don't make that any other valid name, hence the <> */        \
        s << "<unknown>";                                               \
        break;                                                          \
    }                                                                   \
    return s;                                                           \
  }                                                                     \

#define ELLE_ENUM_VALUE(R, Name, Elem)                                  \
  BOOST_PP_TUPLE_ELEM(2, 0, Elem) = BOOST_PP_TUPLE_ELEM(2, 1, Elem),    \

#define ELLE_ENUM_PRINT(R, Name, Elem)                                  \
  case Name::BOOST_PP_TUPLE_ELEM(2, 0, Elem):                           \
    s << BOOST_PP_STRINGIZE(BOOST_PP_TUPLE_ELEM(2, 0, Elem));           \
    break;
