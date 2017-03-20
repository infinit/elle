#ifndef ELLE_SYSTEM_SIGNAL_HH
# define ELLE_SYSTEM_SIGNAL_HH

# include <string>

namespace elle
{
  namespace system
  {
    /// Get the name of the given signal.
    std::string
    strsignal(int signum);
  } /* system */
} /* elle */

#endif /* end of include guard: ELLE_SYSTEM_SIGNAL_HH*/
