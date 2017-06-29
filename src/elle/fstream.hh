#pragma once

#include <fstream>

#include <boost/filesystem.hpp>

#include <elle/bytes.hh>
#include <elle/print.hh>
#include <elle/printf.hh> // for err

namespace bfs = boost::filesystem;

namespace elle
{
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
      // Look for the first free name.
      for (int i = 0;; ++i)
      {
        // There is no O_EXCL in C++.  TOCTOU...
        auto f = elle::print("{}.{}", base, i);
        if (!bfs::exists(f))
        {
          of.open(f, std::ios_base::app);
          if (of.good())
            return;
        }
      }
      err("failed to open file in family %s", base);
    }
  }
}
