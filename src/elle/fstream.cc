#include <elle/fstream.hh>

namespace elle
{
  namespace detail
  {
    template
    void
    rotate_impl<char>(std::ofstream& of, std::string const& base, int threshold);
  }

  template
  void
  rotate(std::ofstream& of, std::string const& base, int threshold);
}
