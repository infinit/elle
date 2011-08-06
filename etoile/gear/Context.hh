//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/gear/Context.hh
//
// created       julien quintard   [thu jun 16 09:55:55 2011]
// updated       julien quintard   [fri aug  5 12:40:34 2011]
//

#ifndef ETOILE_GEAR_CONTEXT_HH
#define ETOILE_GEAR_CONTEXT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/gear/Nature.hh>
#include <etoile/gear/Transcript.hh>

namespace etoile
{
  namespace gear
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// an context contains the information related to a set of blocks.
    ///
    /// automata are then triggered on these contexts in order to perform
    /// some modifications.
    ///
    class Context:
      public elle::Object
    {
    public:
      //
      // enumerations
      //
      enum State
	{
	  StateUnknown,

	  StateInitialized,

	  StateModified,

	  StateDiscarded,
	  StateStored,
	  StateDestroyed,

	  StateCleaned
	};

      //
      // constructors & destructors
      //
      Context(const Nature);

      //
      // interfaces
      //

      // dumpable
      elle::Status	Dump(const elle::Natural32 = 0) const;

      // archivable
      elle::Status	Serialize(elle::Archive&) const;
      elle::Status	Extract(elle::Archive&);

      //
      // attributes
      //
      Nature		nature;

      State		state;

      Transcript	transcript;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <etoile/gear/Context.hxx>

#endif
