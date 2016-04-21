#ifndef ELLE_ERR_HH
# define ELLE_ERR_HH

# include <iosfwd>
# include <utility>

# include <elle/printf-fwd.hh>

namespace elle
{
  template <typename ... Args>
  void
  err(std::string const& fmt, Args&& ... args)
  {
    elle::err(sprintf(fmt, std::forward<Args>(args)...));
  }

  void
  err(std::string const& msg);
}

#endif
