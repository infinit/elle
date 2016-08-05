#ifndef ELLE_UNISTD_HH
# define ELLE_UNISTD_HH

# ifndef INFINIT_WINDOWS
#  include <sys/types.h>

# include <string>

namespace elle
{
  void
  chown(std::string const& pathname, uid_t owner, gid_t group);
  void
  setegid(gid_t egid);
  void
  seteuid(uid_t euid);
  void
  setgid(gid_t gid);
  void
  setuid(uid_t uid);
}

# endif
#endif
