#ifndef ELLE_UNISTD_HH
# define ELLE_UNISTD_HH

# ifndef INFINIT_WINDOWS
#  include <sys/types.h>

# include <string>

namespace elle
{
  /// Change directory.
  ///
  /// @param path The new path.
  void
  chdir(char const* path);
  /// Change directory.
  ///
  /// @param fd The file descriptor.
  void
  fchdir(int fd);
  /// Change the owner of \a path.
  ///
  /// @param path The path.
  /// @param owner The uid of the new owner.
  /// @param owner The uid of the new group.
  void
  chown(std::string const& path, uid_t owner, gid_t group);
  /// Change our egid.
  ///
  /// @param egid Our new egid.
  void
  setegid(gid_t egid);
  /// Change our eduid.
  ///
  /// @param euid Our new euid.
  void
  seteuid(uid_t euid);
  /// Change our gid
  ///
  /// @param gid Our new gid.
  void
  setgid(gid_t gid);
  /// Change our uid.
  ///
  /// @param uid Our new uid.
  void
  setuid(uid_t uid);
}

# endif
#endif
