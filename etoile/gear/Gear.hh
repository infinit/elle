//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/gear/Gear.hh
//
// created       julien quintard   [fri jun  3 10:58:40 2011]
// updated       julien quintard   [fri jul 29 14:28:30 2011]
//

#ifndef ETOILE_GEAR_GEAR_HH
#define ETOILE_GEAR_GEAR_HH

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/gear/Identifier.hh>
#include <etoile/gear/Scope.hh>

#include <elle/idiom/Close.hh>
# include <map>
#include <elle/idiom/Open.hh>

namespace etoile
{
  ///
  /// this namespace contains everything for recording operations on
  /// the file system so that such operations could be replayed for instance.
  ///
  namespace gear
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    class Gear
    {
    public:
      static elle::Status	Initialize();
      static elle::Status	Clean();
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/gear/Action.hh>
#include <etoile/gear/Actor.hh>
#include <etoile/gear/Chronicle.hh>
#include <etoile/gear/Context.hh>
#include <etoile/gear/Directory.hh>
#include <etoile/gear/Event.hh>
#include <etoile/gear/File.hh>
#include <etoile/gear/Identifier.hh>
#include <etoile/gear/Link.hh>
#include <etoile/gear/Nature.hh>
#include <etoile/gear/Object.hh>
#include <etoile/gear/Operation.hh>
#include <etoile/gear/Scope.hh>
#include <etoile/gear/State.hh>
#include <etoile/gear/Transaction.hh>
#include <etoile/gear/Transcript.hh>

#endif
