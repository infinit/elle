#include <etoile/gear/Gear.hh>
#include <etoile/gear/Actor.hh>
#include <etoile/gear/Scope.hh>

#include <elle/idiom/Open.hh>

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
      // initialize the actor system.
      if (Actor::Initialize() == elle::Status::Error)
        escape("unable to initialize the actor");

      // initialize the scope system.
      if (Scope::Initialize() == elle::Status::Error)
        escape("unable to initialize the scope");

      return elle::Status::Ok;
    }

    ///
    /// this method cleans the gear system.
    ///
    elle::Status        Gear::Clean()
    {
      // clean the actor system.
      if (Actor::Clean() == elle::Status::Error)
        escape("unable to clean the actor");

      // clean the scope system.
      if (Scope::Clean() == elle::Status::Error)
        escape("unable to clean the scope");

      return elle::Status::Ok;
    }

  }
}
