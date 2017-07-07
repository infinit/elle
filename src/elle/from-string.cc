#include <elle/from-string.hh>

#include <unordered_map>

#include <boost/algorithm/string/case_conv.hpp>

#include <elle/err.hh>
#include <elle/find.hh>
#include <elle/printf.hh> // for elle::err

namespace elle
{
  bool
  from_string(std::string const& s, bool*)
  {
    static auto const map = std::unordered_map<std::string, bool>
      {
        {"0",     false},
        {"1",     true},
        {"false", false},
        {"n",     false},
        {"no",    false},
        {"true",  true},
        {"y",     true},
        {"yes",   true},
      };
    if (auto it = elle::find(map, boost::to_lower_copy(s)))
      return it->second;
    else
      elle::err("invalid boolean value: %s", s);
  }
}
