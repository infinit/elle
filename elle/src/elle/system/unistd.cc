#ifndef INFINIT_WINDOWS
# include <errno.h>
# include <string.h>
# include <sys/types.h>
# include <unistd.h>

# include <string>

# include <elle/err.hh>
# include <elle/printf.hh>
# include <elle/system/unistd.hh>

namespace elle
{
  static
  int
  checked_call(int res, std::string const& syscall)
  {
    if (res == -1)
      elle::err("unable to %s: %s", syscall, strerror(errno));
    return res;
  }

  void
  setegid(gid_t egid)
  {
    checked_call(::setegid(egid), "setegid");
  }

  void
  seteuid(uid_t euid)
  {
    checked_call(::seteuid(euid), "seteuid");
  }

  void
  setgid(gid_t gid)
  {
    checked_call(::setgid(gid), "setgid");
  }

  void
  setuid(uid_t uid)
  {
    checked_call(::setuid(uid), "setuid");
  }
}

#endif

