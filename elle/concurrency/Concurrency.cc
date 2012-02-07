//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [tue mar 23 15:05:31 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/concurrency/Concurrency.hh>

namespace elle
{
  namespace concurrency
  {

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method initializes the concurrency module.
    ///
    Status              Concurrency::Initialize()
    {
      ;

      // initialize the program.
      if (Program::Initialize() == StatusError)
        escape("unable to initialize the program");

      // initialize the fiber system.
      if (Fiber::Initialize() == StatusError)
        escape("unable to initialize the fiber system");

      return elle::StatusOk;
    }

    ///
    /// this method cleans the concurrency module.
    ///
    Status              Concurrency::Clean()
    {
      ;

      // clean the fiber system.
      if (Fiber::Clean() == StatusError)
        escape("unable to clean the fiber system");

      // clean the program.
      if (Program::Clean() == StatusError)
        escape("unable to clean the program");

      return elle::StatusOk;
    }

  }
}
