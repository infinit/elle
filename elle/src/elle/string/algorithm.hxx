#include <elle/string/algorithm.hh>

namespace elle
{
  template <typename IT>
  std::string
  join(IT it, IT ite, std::string const& sep,
       std::function<std::string (typename IT::value_type const&)> const& t)

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
      if (t)
        elle::fprintf(out, "%s", t(val));
      else
        elle::fprintf(out, "%s", val);
    }
    return out.str();
  }


  template <typename C>
  std::string
  join(C const& container,
       std::string const& sep,
       std::function<std::string (typename C::const_reference)> const& t)
  {
    return join(container.begin(), container.end(), sep, t);
  }
} /* elle */
