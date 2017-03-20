#pragma once

#include <cstddef>

#include <boost/preprocessor/cat.hpp>

namespace elle
{
  namespace sfinae
  {
    template<size_t>
    struct Helper
    {};
  }
}

/// Helpers for template arguments for SFINAE operations.

#define ELLE_SFINAE_IF_WORKS(Expr)                      \
  int, ::elle::sfinae::Helper<sizeof(Expr, 0)>* = nullptr

#define ELLE_SFINAE_IF_POSSIBLE()               \
  int

#define ELLE_SFINAE_OTHERWISE()                 \
  unsigned int

#define ELLE_SFINAE_TRY()                       \
  42

#define ELLE_STATIC_PREDICATE(Name, Test)                               \
  template <typename T>                                                 \
  constexpr                                                             \
  std::enable_if_exists_t<Test, bool>                                   \
  BOOST_PP_CAT(_, Name)(int)                                            \
  {                                                                     \
    return true;                                                        \
  }                                                                     \
                                                                        \
  template <typename T>                                                 \
  constexpr                                                             \
  bool                                                                  \
  BOOST_PP_CAT(_, Name)(...)                                            \
  {                                                                     \
    return false;                                                       \
  }                                                                     \
                                                                        \
  template <typename T>                                                 \
  constexpr                                                             \
  bool                                                                  \
  Name()                                                                \
  {                                                                     \
    return BOOST_PP_CAT(_, Name)<T>(42);                                \
  }
