#ifndef ELLE_FORMAT_JSON_INTEGER_HH
# define ELLE_FORMAT_JSON_INTEGER_HH

# include "Object.hh"
# include "_detail.hh"
# include "_internal.hh"

namespace elle
{
  namespace format
  {
    namespace json
    {

      typedef detail::BasicObject<internal::Integer> Integer;

    }
  }
} // !namespace elle::format::json

#endif /* ! JSON_INT_HH */
