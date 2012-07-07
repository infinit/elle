#ifndef  COMMON_COMMON_HH
# define COMMON_COMMON_HH

# include <string>

# define COMMON_DEFAULT_INFINIT_HOME ".config/infinit"

namespace common
{

  std::string const& home_directory();
  std::string const& infinit_home();

}

#endif

