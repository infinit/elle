#ifndef  ELLE_FORMAT_HH
# define ELLE_FORMAT_HH

namespace elle
{
  ///
  /// Format values into a string as the C `sprintf' or the `format' and
  /// returns it.
  ///
  /// @see http://www.boost.org/doc/libs/1_49_0/libs/format/
  ///
  template<typename... T>
  std::string
    format(char const* fmt, T const&... values);

}

# include <elle/format.hxx>

#endif

