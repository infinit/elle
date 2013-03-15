#ifndef ELLE_JSON_BOOL_HH
# define ELLE_JSON_BOOL_HH

# include "Object.hh"
# include "_detail.hh"
# include "_internal.hh"

namespace elle
{
  namespace format
  {
    namespace json
    {

      typedef detail::BasicObject<internal::Bool> Bool;

      extern Bool const true_;
      extern Bool const false_;

    }
  }
} // !namespace elle::format::json

#endif /* ! BOOL_HH */
