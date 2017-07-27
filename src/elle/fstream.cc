#include <elle/fstream.hh>

#include <boost/range/algorithm/sort.hpp>

#include <elle/algorithm.hh>
#include <elle/from-string.hh>

namespace elle
{
  std::vector<int>
  rotate_versions(bfs::path const& base)
  {
    auto res = std::vector<int>{};
    auto const dir = base.parent_path().empty() ? "." : base.parent_path();
    // Use `generic_string` to avoid gratuitious differences on `/` vs `\`.
    auto const prefix = base.generic_string() + ".";
    for (auto& p: bfs::directory_iterator(dir))
      if (auto n = elle::tail(p.path().generic_string(), prefix))
        try
        {
          res.emplace_back(from_string<int>(*n));
        }
        catch (std::invalid_argument)
        {}
    boost::sort(res);
    return res;
  }


  /*--------------------------.
  | Explicit instantiations.  |
  `--------------------------*/
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
    rotate_impl<char>(std::ofstream& of, std::string const& base,
                      int size, int rotate, bool append);
  }

  template
  void
  rotate(std::ofstream& of, std::string const& base,
         int size, int rotate, bool append);
}
