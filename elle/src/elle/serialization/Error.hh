#ifndef ELLE_SERIALIZATION_ERROR_HH
# define ELLE_SERIALIZATION_ERROR_HH

# include <elle/Exception.hh>

namespace elle
{
  namespace serialization
  {
    class Error:
      public elle::Exception
    {
    public:
      Error(std::string const& message);
    };
  }
}

#endif
