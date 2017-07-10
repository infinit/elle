#pragma once

#include <fstream>
#include <vector>

#include <boost/range/adaptor/sliced.hpp>

#include <elle/bytes.hh>
#include <elle/filesystem.hh>
#include <elle/log.hh>
#include <elle/print.hh>
#include <elle/printf.hh> // for err

ELLE_LOG_COMPONENT("elle.fstream");

namespace elle
{
  /// The sorted list of existing versions of a file family.
  ///
  /// For instance /tmp/foo.1, /tmp/foo.42, /tmp/foo.5 -> (1, 5, 42).
  /// Very alike glob(base.*).
  std::vector<int>
  rotate_versions(bfs::path const& base);

  namespace detail
  {
    /// Rotate the file of a stream.
    ///
    /// @param of      the output stream.
    /// @param base    the base for file names.
    /// @param rotate  how many files to keep (0 for unlimited).
    template <typename CharT, typename Traits = std::char_traits<CharT>>
    void
    rotate_impl(std::basic_ofstream<CharT, Traits>& of,
                std::string const& base,
                int rotate)
    {
      auto const vs = rotate_versions(base);
      auto const next = vs.empty() ? 0 : vs.back() + 1;
      // Remove the files that are too old.
      if (rotate && rotate < int(vs.size() + 1))
        for (auto i: vs | boost::adaptors::sliced(0, vs.size() + 1 - rotate))
          try_remove(elle::print("{}.{}", base, i));
      // Look for the first free name.
      for (int i = next; i < next + 10000; ++i)
      {
        // There is no O_EXCL in C++.  TOCTOU...
        auto f = elle::print("{}.{}", base, i);
        if (!bfs::exists(f))
        {
          of.open(f, std::ios_base::app);
          if (of.good())
          {
            ELLE_DUMP("rotate_name: {}", f);
            return;
          }
        }
      }
      err("failed to open file in family %s", base);
    }
  }

  /// Rotate the file of a stream.
  ///
  /// @param of      the stream
  /// @param base    the base name for the file names
  /// @param size    the threshold beyond which files are rotated.
  /// @param rotate  how many files to keep (0 to unlimited).
  template <typename CharT, typename Traits = std::char_traits<CharT>>
  void
  rotate(std::basic_ofstream<CharT, Traits>& of,
         std::string const& base,
         int size = 1_MiB,
         int rotate = 0)
  {
    if (of.is_open() && size < of.tellp())
    {
      of.close();
      if (of.fail())
        err("failed to close file in family %s", base);
    }
    if (!of.is_open())
      detail::rotate_impl(of, base, rotate);
  }


  /*--------------------------.
  | Explicit instantiations.  |
  `--------------------------*/
  namespace detail
  {
    extern template
    void
    rotate_impl(std::ofstream& of, std::string const& base, int rotate);
  }
  extern template
  void
  rotate(std::ofstream& of, std::string const& base, int size, int rotate);
}
