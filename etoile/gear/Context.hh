//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien quintard   [thu jun 16 09:55:55 2011]
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
    /// an context contains the information related to a set of blocks
    /// linked together.
    ///
    /// automata are then triggered on these contexts in order to perform
    /// modifications.
    ///
    /// every context has a state indicating its process stage.
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

#endif
