#ifndef ELLE_CONCURRENCY_PROGRAM_HH
# define ELLE_CONCURRENCY_PROGRAM_HH

# include <elle/radix/Entity.hh>
# include <elle/radix/Parameters.hh>

# include <elle/concurrency/Callback.hh>
# include <elle/concurrency/Broker.hh>

#include <elle/idiom/Close.hh>

# include <QCoreApplication>
# include <QAbstractEventDispatcher>
# include <list>
# include <signal.h>

#include <elle/idiom/Open.hh>

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
      static Status             Launch();
      static Void               Exception(int);
      static Status             Exit();

#if defined(INFINIT_LINUX) || defined(INFINIT_MACOSX)
      static Status             Attach(Broker&);
      static Status             Detach(Broker&);
#elif defined(INFINIT_WINDOWS)
      // nothing
#else
# error "unsupported platform"
#endif

      //
      // constructors & destructors
      //
      Program();
      ~Program();

      //
      // attributes
      //
      ::QCoreApplication*       core;

      State                     state;
    private:
      static bool               _exit;
      friend void qt_runner();
    };

//
// ---------- externs ---------------------------------------------------------
//

    extern Program*             program;
  }
}

#endif
