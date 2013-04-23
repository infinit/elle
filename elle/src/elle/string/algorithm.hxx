#include <elle/string/algorithm.hh>

namespace elle
{
  template <typename IT>
  std::string join(IT it, IT ite, std::string const& sep)
  {
    int n = 0;
    std::stringstream res;
    for (; it != ite; ++it)
    {
      auto const& val = *it;

      if (n++)
        res << sep;
      res << val;
    }
    return res.str();
  }
} /* elle */
