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
      // initialize the program.
      if (Program::Initialize() == Status::Error)
        escape("unable to initialize the program");

      return Status::Ok;
    }

    ///
    /// this method cleans the concurrency module.
    ///
    Status              Concurrency::Clean()
    {
      // clean the program.
      if (Program::Clean() == Status::Error)
        escape("unable to clean the program");

      return Status::Ok;
    }

  }
}
