#ifndef ELLE_SFINAE_HH
# define ELLE_SFINAE_HH

# include <cstddef>

namespace elle
{
  namespace sfinae
  {
    template<size_t>
    struct Helper
    {};
  }
}

# define ELLE_SFINAE_IF_WORKS(Expr)                             \
  int, ::elle::sfinae::Helper<sizeof(Expr, 0)>* = 0

# define ELLE_SFINAE_IF_POSSIBLE()                             \
  int

# define ELLE_SFINAE_OTHERWISE()                \
  unsigned int

# define ELLE_SFINAE_TRY()                      \
  42

# define ELLE_SFINAE_INSTANCE(Type)             \
  (*reinterpret_cast<typename std::remove_reference<Type>::type*>((void*)(0)))

#endif
