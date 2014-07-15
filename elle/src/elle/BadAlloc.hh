#ifndef ELLE_BAD_ALLOC_HH
# define ELLE_BAD_ALLOC_HH

# include <elle/Error.hh>

namespace elle
{
  class BadAlloc
    : public Error
  {
  public:
    BadAlloc();
  };
}

#endif
