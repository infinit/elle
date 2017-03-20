#ifndef ELLE_WINDOWS_STRING_CONVERSION_HH
# define ELLE_WINDOWS_STRING_CONVERSION_HH

# include <string>

# ifndef INFINIT_WINDOWS
#  error "This file should only be included on Windows"
#  error "(if INFINIT_WINDOWS is defined)."
# endif

namespace elle
{
  namespace string
  {
    /// Turn an std::string encoded with code page to be determined to an UTF-8
    /// string.
    std::string
    windows_encoding_to_utf8(std::string const& string);

    /// Turn an UTF-8 std::string to a std::string with the right code page.
    std::string
    utf8_to_windows_encoding(std::string const& string);

    /// Turn wstring to a string.
    ///
    /// Use WideCharToMultiByte.
    std::string
    wstring_to_string(std::wstring const& s);
  }
}

#endif
