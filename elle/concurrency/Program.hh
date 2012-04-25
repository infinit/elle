#ifndef ELLE_CONCURRENCY_PROGRAM_HH
# define ELLE_CONCURRENCY_PROGRAM_HH

# include <list>
# include <signal.h>

# include <elle/concurrency/Callback.hh>

# include <elle/idiom/Close.hh>
#  include <elle/radix/Entity.hh>
#  include <elle/radix/Parameters.hh>
#  include <elle/radix/Status.hh>
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
    class Program:
      public Entity
    {
    public:
      //
      // enumerations
      //
      enum State
        {
          StateUnknown,
          StateActive,
          StateStopped
        };

      //
      // static methods
      //
      static Status             Initialize();
      static Status             Clean();

      static Status             Setup();
      static void               Launch();
      static Void               Exception(int);
      static void               Exit();

      //
      // constructors & destructors
      //
      Program();
      ~Program();

      //
      // attributes
      //
      State                     state;
    private:
      static reactor::Signal    _exit;
    };

//
// ---------- externs ---------------------------------------------------------
//

    extern Program*             program;
  }
}

#endif
