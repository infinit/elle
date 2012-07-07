#ifndef ELLE_CONCURRENCY_SCHEDULER_HH
# define ELLE_CONCURRENCY_SCHEDULER_HH

# include <elle/idiom/Close.hh>
#  include <reactor/scheduler.hh>
# include <elle/idiom/Open.hh>

// XXX[revoir ce fichier?]

namespace elle
{
  namespace concurrency
  {
    reactor::Scheduler& scheduler();
  }
}

#endif
