#ifndef ELLE_ERROR_HH
# define ELLE_ERROR_HH

# include <elle/Exception.hh>

namespace elle
{
  /// Exception for runtime errors.
  class Error
    : public Exception
  {
  /*------.
  | Types |
  `------*/
  public:
    typedef Error Self;
    typedef Exception Super;

  /*-------------.
  | Construction |
  `-------------*/
  public:
    using Super::Super;
  };
}

#endif
