#ifndef HORIZON_FWD_HH
# define HORIZON_FWD_HH

# include <elle/system/platform.hh>

# if defined(INFINIT_LINUX)
#  include <horizon/linux/fwd.hh>
# elif defined(INFINIT_MACOSX)
#  include <horizon/macosx/fwd.hh>
# elif defined(INFINIT_WINDOWS)
#  include <horizon/windows/fwd.hh>
# else
#  error "unsupported platform"
# endif

# include <horizon/Policy.hh>

namespace horizon
{

  // XXX

}

#endif
