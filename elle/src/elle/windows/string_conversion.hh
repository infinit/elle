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
    std::string
    windows_encoding_to_utf8(std::string const& string);

    std::string
    utf8_to_windows_encoding(std::string const& string);

    std::string
    wstring_to_string(std::wstring const& s);
  }
}

#endif
