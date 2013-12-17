#ifndef ELLE_OS_EXCEPTIONS_HH
# define ELLE_OS_EXCEPTIONS_HH

# include <elle/exception.hh>

namespace elle
{
  namespace os
  {
    class KeyError:
      public elle::Exception
    {
    public:
      KeyError(std::string const& key);
    };
  }
}

#endif // ELLE_OS_EXCEPTIONS_HH