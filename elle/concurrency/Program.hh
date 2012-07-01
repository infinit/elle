#ifndef ELLE_CONCURRENCY_PROGRAM_HH
# define ELLE_CONCURRENCY_PROGRAM_HH

#  include <elle/radix/Entity.hh>
#  include <elle/radix/Parameters.hh>

# include <elle/concurrency/Callback.hh>

# include <elle/idiom/Close.hh>
#  include <elle/types.hh>
#  include <list>
#  include <signal.h>
# include <elle/idiom/Open.hh>

namespace reactor
{
  class Scheduler;
}

namespace elle
{
  using namespace radix;

  namespace concurrency
  {
//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents the running program.
    ///
    class Program
    {
    public:
      //
      // static methods
      //
      static Status             Setup();
      static void               Launch();
      static Void               Exception(int);
      static void               Exit();

      //
      // attributes
      //
    private:
      static reactor::Signal    _exit;
    };

  }
}

#endif
