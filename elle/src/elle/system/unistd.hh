#ifndef ELLE_UNISTD_HH
# define ELLE_UNISTD_HH

# ifndef INFINIT_WINDOWS
#  include <sys/types.h>

namespace elle
{
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
