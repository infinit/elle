#pragma once

#include <fstream>
#include <vector>

#include <boost/range/adaptor/sliced.hpp>
#include <boost/range/irange.hpp>

#include <elle/bytes.hh>
#include <elle/filesystem.hh>
#include <elle/log.hh>
#include <elle/print.hh>
#include <elle/printf.hh> // for err

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
    /// @param size    the threshold beyond which files are rotated.
    /// @param rotate  how many files to keep (0 for unlimited).
    /// @param append  whether to append to the latest file,
    ///                rather than creating a new one.
    template <typename CharT, typename Traits = std::char_traits<CharT>>
    void
    rotate_impl(std::basic_ofstream<CharT, Traits>& of,
                std::string const& base,
                int size, int rotate, bool append)
    {
      ELLE_LOG_COMPONENT("elle.fstream");
      auto const vs = rotate_versions(base);
      auto const last = vs.empty() ? -1 : vs.back();
      // Remove the files that are too old.  If we don't append, we
      // will create a new file, so remove one more file.
      if (rotate && rotate < int(vs.size() + !append))
        for (auto i: vs | boost::adaptors::sliced(0, vs.size() + 1 - rotate))
          try_remove(elle::print("{}.{}", base, i));
      // If append is enabled, try to reuse the last file.
      if (append
          && last != -1
          && int(file_size(bfs::path(elle::print("{}.{}", base, last)))) < size)
      {
        auto const f = elle::print("{}.{}", base, last);
        of.open(f, std::ios_base::app);
        if (of.good())
        {
          ELLE_DUMP("rotate_name: {}", f);
          return;
        }
      }
      // Look for the first free name.
      auto const next = last + 1;
      for (auto i: boost::irange(next, next + 10000))
      {
        // There is no O_EXCL in C++.  TOCTOU...
        auto const f = elle::print("{}.{}", base, i);
        if (!bfs::exists(f))
        {
          of.open(f);
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
  /// @param append  whether to append to the latest file,
  ///                rather than creating a new one.
  template <typename CharT, typename Traits = std::char_traits<CharT>>
  void
  rotate(std::basic_ofstream<CharT, Traits>& of,
         std::string const& base,
         int size = 1_MiB,
         int rotate = 0,
         bool append = false)
  {
    if (of.is_open() && size <= of.tellp())
    {
      of.close();
      if (of.fail())
        err("failed to close file in family %s", base);
    }
    if (!of.is_open())
      detail::rotate_impl(of, base, size, rotate, append);
  }


  /*--------------------------.
  | Explicit instantiations.  |
  `--------------------------*/
  namespace detail
  {
    extern template
    void
    rotate_impl(std::ofstream& of, std::string const& base,
                int size, int rotate, bool append);
  }
  extern template
  void
  rotate(std::ofstream& of, std::string const& base,
         int size, int rotate, bool append);
}
