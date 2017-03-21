#pragma once

#include <boost/multi_index_container.hpp>

namespace boost
{
  namespace multi_index
  {
    /// Pretty-printing.
    template <typename... Args>
    std::ostream&
    operator <<(std::ostream& out, multi_index_container<Args...> const& s)
    {
      auto const format = is_fixed(out) ? "%s%f" : "%s%s";
      out << "BMI{";
      auto* sep = "";
      for (auto const& e: s)
      {
        elle::fprintf(out, format, sep, e);
        sep = ", ";
      }
      out << '}';
      return out;
    }
  }
}
