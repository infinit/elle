#ifndef ELLE_FORMAT_JSON__INTERNAL_HH
# define ELLE_FORMAT_JSON__INTERNAL_HH

# include <string>
# include <unordered_map>
# include <vector>

# include "fwd.hh"

namespace elle
{
  namespace format
  {
    namespace json
    {

      class Object;

      namespace internal
      {
        ///
        /// Internal types.
        ///

        typedef bool                                      Bool;
        typedef int32_t                                   Integer;
        typedef double                                    Float;
        typedef std::string                               String;
        typedef std::vector<Object*>                      Array;
        typedef std::unordered_map<std::string, Object*>  Dictionary;

      }
    }
  }
}

#endif

