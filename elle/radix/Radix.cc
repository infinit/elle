//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [tue apr 27 12:11:16 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/radix/Radix.hh>

#include <elle/standalone/Report.hh>

#include <elle/idiom/Open.hh>

namespace elle
{
  namespace radix
  {

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method initializes the radix module.
    ///
    Status              Radix::Initialize()
    {
      // initialize the meta class.
      if (Meta::Initialize() == Status::Error)
        escape("unable to initialize the meta class");

      return Status::Ok;
    }

    ///
    /// this method cleans the radix module.
    ///
    Status              Radix::Clean()
    {
      // clean the meta class.
      if (Meta::Clean() == Status::Error)
        escape("unable to clean the meta class");

      return Status::Ok;
    }

  }
}
