#ifndef ELLE_JSON_EXCEPTIONS_HH
# define ELLE_JSON_EXCEPTIONS_HH

# include <elle/Error.hh>

namespace elle
{
  namespace json
  {
    class ParseError:
      public elle::Error
    {
    public:
      typedef elle::Error Super;
      ParseError(std::string const& message);
    };
  }
}

#endif
