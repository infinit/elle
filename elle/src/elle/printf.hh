#ifndef  ELLE_PRINTF_HH
# define ELLE_PRINTF_HH

# include <iosfwd>
# include <string>

namespace elle
{
  template <typename F, typename... T>
  size_t
  printf(F&& fmt,
         T&&... values);

  template <typename F, typename... T>
  size_t
  fprintf(std::ostream& out,
          F&& fmt,
          T&&... values);

  template <typename F, typename... T>
  std::string
  sprintf(F&& fmt,
          T&&... values);
}

# include <elle/printf.hxx>

#endif
