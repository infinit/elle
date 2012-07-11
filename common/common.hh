#ifndef  COMMON_COMMON_HH
# define COMMON_COMMON_HH

# include <string>

# define COMMON_DEFAULT_INFINIT_HOME ".config/infinit"

namespace common
{

  /// Returns the user home directory.
  std::string const& home_directory();

  /// Returns infinit home directory.
  /// Defaults to ~/.config/infinit on unices but could be changed by exporting
  /// INFINIT_HOME variable.
  std::string const& infinit_home();

  /// Returns binary path from its name.
  /// Defaults to ~/.config/infinit/bin/binary_name but if INFINIT_BUILD_DIR is
  /// exported, it will return the accurate binary path inside the build
  /// directory.
  /// Raises an exception when ensure is true and the path does not refer to a
  /// valid binary.
  std::string const& binary_path(std::string const& name, bool ensure = true);

  /// Returns the path to the passport file.
  std::string const& passport_path();
}

#endif

