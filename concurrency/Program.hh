#ifndef ELLE_CONCURRENCY_PROGRAM_HH
# define ELLE_CONCURRENCY_PROGRAM_HH

# include <elle/types.hh>

# include <reactor/signal.hh>

// XXX
namespace reactor
{
  class Scheduler;
}

namespace elle
{
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
