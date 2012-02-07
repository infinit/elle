//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien quintard   [tue jun 14 16:41:06 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/gear/Gear.hh>

namespace etoile
{
  namespace gear
  {

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method initializes the gear system.
    ///
    elle::Status        Gear::Initialize()
    {
      ;

      // initialize the actor system.
      if (Actor::Initialize() == elle::StatusError)
        escape("unable to initialize the actor");

      // initialize the scope system.
      if (Scope::Initialize() == elle::StatusError)
        escape("unable to initialize the scope");

      return elle::StatusOk;
    }

    ///
    /// this method cleans the gear system.
    ///
    elle::Status        Gear::Clean()
    {
      ;

      // clean the actor system.
      if (Actor::Clean() == elle::StatusError)
        escape("unable to clean the actor");

      // clean the scope system.
      if (Scope::Clean() == elle::StatusError)
        escape("unable to clean the scope");

      return elle::StatusOk;
    }

  }
}
