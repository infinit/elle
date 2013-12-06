#ifndef ELLE_JSON_EXCEPTIONS_HH
# define ELLE_JSON_EXCEPTIONS_HH

# include <elle/Exception.hh>

namespace elle
{
  namespace json
  {
    class ParserError:
      public elle::Exception
    {
    public:
      ParserError(std::string const& message);
    };
  }
}

#endif
