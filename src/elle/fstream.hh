#pragma once

#include <fstream>

#include <boost/filesystem.hpp>

#include <elle/bytes.hh>
#include <elle/log.hh>
#include <elle/print.hh>
#include <elle/printf.hh> // for err

namespace bfs = boost::filesystem;

ELLE_LOG_COMPONENT("elle.fstream");

namespace elle
{
  template <typename CharT = char,
            typename Traits = std::char_traits<CharT>>
  std::basic_ofstream<CharT, Traits>
  rotate_name(std::string const& base,
              int threshold = 100_KiB)
  {
    using ofstream = std::basic_ofstream<CharT, Traits>;
    // Look for the first free name.
    for (int i = 0; i < 100; ++i)
    {
      // There is no O_EXCL in C++.  TOCTOU...
      auto f = elle::print("{}.{}", base, i);
      if (!bfs::exists(f))
      {
        auto&& o = ofstream{f, std::ios_base::app};
        if (o.good())
        {
          ELLE_DUMP("rotate_name: {}", f);
          return std::move(o);
        }
      }
    }
    err("failed to open file in family %s", base);
  }

  /// Rotate the file of a stream.
  ///
  /// \param of    the stream
  /// \param base  the base name for the file names
  /// \param threshold  size starting which the previous file is closed
  template <typename CharT, typename Traits>
  void
  rotate(std::basic_ofstream<CharT, Traits>& of,
         std::string const& base,
         int threshold = 100_KiB)
  {
    if (threshold < of.tellp())
    {
      of.close();
      if (of.fail())
        err("failed to close file in family %s", base);
      of = rotate_name(base, threshold);
    }
  }
}
