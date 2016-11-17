#ifndef ELLE_ERR_HH
# define ELLE_ERR_HH

# include <iosfwd>
# include <utility>

# include <elle/compiler.hh>
# include <elle/printf-fwd.hh>

namespace elle
{
  ELLE_API
  ELLE_COMPILER_ATTRIBUTE_NORETURN
  void
  err(std::string const& msg);

  template <typename ... Args>
  ELLE_COMPILER_ATTRIBUTE_NORETURN
  inline
  void
  err(std::string const& fmt, Args&& ... args)
  {
    elle::err(sprintf(fmt, std::forward<Args>(args)...));
  }
}

#endif
