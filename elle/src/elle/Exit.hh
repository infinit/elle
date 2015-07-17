#ifndef ELLE_EXIT_HH
# define ELLE_EXIT_HH

# include <elle/Exception.hh>

namespace elle
{
  class Exit
    : public Exception
  {
  /*------.
  | Types |
  `------*/
  public:
    typedef Exception Super;
    typedef Exit Self;

  /*-------------.
  | Construction |
  `-------------*/
  public:
    Exit(int return_code);
    ELLE_ATTRIBUTE_R(int, return_code);
  };
}

#endif
