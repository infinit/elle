#pragma once

#include <fstream>
#include <vector>

#include <boost/range/adaptor/sliced.hpp>
#include <boost/range/irange.hpp>

#include <elle/bytes.hh>
#include <elle/filesystem.hh>
#include <elle/filesystem/path.hh>
#include <elle/log.hh>
#include <elle/print.hh>
#include <elle/printf.hh> // for err

namespace elle
{
  /*-----------------.
  | basic_ofstream.  |
  `-----------------*/

  /// An ofstream that knows its file name, and can be renamed.
  template <typename CharT, typename Traits = std::char_traits<CharT>>
  class basic_ofstream
    : public std::basic_ofstream<CharT, Traits>
  {
  public:
    using Self = basic_ofstream;
    using Super = std::basic_ofstream<CharT, Traits>;
    using Super::Super;

    basic_ofstream() {};

    basic_ofstream(fs::path path)
      : Super{path.string()}
      , _path(std::move(path))
    {}

    /// Rename this file.
    void path(fs::path path)
    {
      if (path != this->path())
      {
#ifdef ELLE_WINDOWS
        this->close();
#endif
        fs::rename(this->path(), path);
        this->_path = std::move(path);
#ifdef ELLE_WINDOWS
        this->open(this->_path, std::ios::app | std::ios::out);
#endif
      }
    }

    /// The base of the version family to which this file belongs.
    /// The base includes the (possible) directory.
    ///
    /// \pre the filename is `<BASE>.<NUM>`, where `<NUM>` is a
    /// decimal number.
    fs::path base() const
    {
      return elle::base(this->path());
    }

    void
    open(fs::path path,
         std::ios_base::openmode mode = std::ios_base::out)
    {
      Super::open(path.string(), mode);
      this->_path = std::move(path);
    }

    ELLE_ATTRIBUTE_R(fs::path, path);
  };

  using ofstream = basic_ofstream<char>;
  using wofstream = basic_ofstream<wchar_t>;


  /*----------.
  | content.  |
  `----------*/

  /// The content of stream @a is.
  template <typename CharT, typename Traits = std::char_traits<CharT>>
  std::basic_string<CharT, Traits>
  content(std::basic_ifstream<CharT, Traits>& is)
  {
    auto&& ss = std::basic_stringstream<CharT, Traits>{};
    ss << is.rdbuf();
    return ss.str();
  }

  /// The content of stream @a is.
  template <typename CharT, typename Traits = std::char_traits<CharT>>
  std::basic_string<CharT, Traits>
  content(std::basic_ifstream<CharT, Traits>&& is)
  {
    auto&& ss = std::basic_stringstream<CharT, Traits>{};
    ss << is.rdbuf();
    return ss.str();
  }

  /// The content of file @a p.
  template <typename CharT = char, typename Traits = std::char_traits<CharT>>
  std::basic_string<CharT, Traits>
  content(fs::path const& p)
  {
    return content(std::basic_ifstream<CharT, Traits>{p.string()});
  }


  /*---------.
  | rotate.  |
  `---------*/

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
    rotate_impl(elle::basic_ofstream<CharT, Traits>& of,
                fs::path const& base,
                int size, int rotate, bool append)
    {
      ELLE_LOG_COMPONENT("elle.fstream");
      auto const version = [&](int i)
        {
          return elle::print("{}.{}", base.string(), i);
        };
      auto const vs = rotate_versions(base);
      auto const last = vs.empty() ? -1 : vs.back();
      // Remove the files that are too old.  If we don't append, we
      // will create a new file, so remove one more file.
      if (rotate && rotate < int(vs.size() + !append))
        for (auto i: vs | boost::adaptors::sliced(0, vs.size() + 1 - rotate))
          try_remove(version(i));
      // If append is enabled, try to reuse the last file.
      if (append
          && last != -1
          && int(file_size(fs::path(version(last)))) < size)
      {
        auto const f = version(last);
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
        auto const f = version(i);
        if (!fs::exists(f))
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

    template <typename CharT, typename Traits = std::char_traits<CharT>>
    void
    rotate_impl(elle::basic_ofstream<CharT, Traits>& of,
                int size, int rotate, bool append)
    {
      rotate_impl(of, base(of.path()), size, rotate, append);
    }
  }

  /// Rotate the file of a stream if needed.
  ///
  /// @param of      the stream
  /// @param base    the base for file names.
  /// @param size    the threshold beyond which files are rotated.
  /// @param rotate  how many files to keep (0 to unlimited).
  /// @param append  whether to append to the latest file,
  ///                rather than creating a new one.
  template <typename CharT, typename Traits = std::char_traits<CharT>>
  void
  rotate(elle::basic_ofstream<CharT, Traits>& of,
         fs::path const& base,
         int size = 1_MiB,
         int rotate = 0,
         bool append = false)
  {
    if (of.is_open() && size <= of.tellp())
    {
      of.close();
      if (of.fail())
        err("failed to close file %s", of.path());
    }
    if (!of.is_open())
      detail::rotate_impl(of, base, size, rotate, append);
  }

  /// Rotate the file of a stream if needed.
  ///
  /// @param of      the stream
  /// @param size    the threshold beyond which files are rotated.
  /// @param rotate  how many files to keep (0 to unlimited).
  /// @param append  whether to append to the latest file,
  ///                rather than creating a new one.
  template <typename CharT, typename Traits = std::char_traits<CharT>>
  void
  rotate(elle::basic_ofstream<CharT, Traits>& of,
         int size = 1_MiB,
         int rotate = 0,
         bool append = false)
  {
    elle::rotate(of, base(of.path()), size, rotate, append);
  }


  /*--------------------------.
  | Explicit instantiations.  |
  `--------------------------*/

  // basic_ofstream.
  extern template
  class basic_ofstream<char>;
  extern template
  class basic_ofstream<wchar_t>;

  // content.
  extern template
  std::basic_string<char>
  content(std::basic_ifstream<char>& is);
  extern template
  std::basic_string<char>
  content(std::basic_ifstream<char>&& is);

  extern template
  std::basic_string<wchar_t>
  content(std::basic_ifstream<wchar_t>& is);
  extern template
  std::basic_string<wchar_t>
  content(std::basic_ifstream<wchar_t>&& is);

  // rotate.
  namespace detail
  {
    extern template
    void
    rotate_impl(elle::ofstream& of, int size, int rotate, bool append);
  }
  extern template
  void
  rotate(elle::ofstream& of, int size, int rotate, bool append);
}
