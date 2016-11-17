#include <elle/string/algorithm.hh>

namespace elle
{
  template <typename F, typename... T>
  std::ostream&
  fprintf(std::ostream& out,
          F&& fmt,
          T&&... values);

  template <typename IT>
  std::string
  join(IT it, IT ite, std::string const& sep)
  {
    std::stringstream out;
    bool first = true;
    for (; it != ite; ++it)
    {
      auto const& val = *it;
      if (first)
        first = false;
      else
        out << sep;
      elle::fprintf(out, "%s", val);
    }
    return out.str();
  }
} /* elle */
