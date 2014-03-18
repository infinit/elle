#ifndef ELLE_SFINAE_HH
# define ELLE_SFINAE_HH

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
  int, ::elle::sfinae::Helper<sizeof(Expr)>* = 0

# define ELLE_SFINAE_OTHERWISE()                \
  unsigned int

# define ELLE_SFINAE_TRY()                      \
  42

#endif
