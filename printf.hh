#ifndef  ELLE_PRINTF_HH
# define ELLE_PRINTF_HH

# include <iosfwd>
# include <string>

namespace elle
{
  template<typename... T>
    size_t printf(char const* fmt, T const&... values);

  template<typename... T>
    size_t fprintf(std::ostream& out, char const* fmt, T const&... values);

  template<typename... T>
    std::string sprintf(char const* fmt, T const&... values);
}

# include <elle/printf.hxx>

#endif

