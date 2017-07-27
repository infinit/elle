#include <elle/fstream.hh>

namespace elle
{

  /*--------------------------.
  | Explicit instantiations.  |
  `--------------------------*/
  // basic_ofstream.
  template
  class basic_ofstream<char>;
  template
  class basic_ofstream<wchar_t>;

  // content.
  template
  std::basic_string<char>
  content(std::basic_ifstream<char>& is);
  template
  std::basic_string<char>
  content(std::basic_ifstream<char>&& is);

  template
  std::basic_string<wchar_t>
  content(std::basic_ifstream<wchar_t>& is);
  template
  std::basic_string<wchar_t>
  content(std::basic_ifstream<wchar_t>&& is);

  // rotate.
  namespace detail
  {
    template
    void
    rotate_impl<char>(elle::ofstream& of, int size, int rotate, bool append);
  }

  template
  void
  rotate(elle::ofstream& of, int size, int rotate, bool append);
}
