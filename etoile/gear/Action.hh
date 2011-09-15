//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien quintard   [fri jun 24 14:45:52 2011]
//

#ifndef ETOILE_GEAR_ACTION_HH
#define ETOILE_GEAR_ACTION_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>

namespace etoile
{
  namespace gear
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class defines an action to be performed by the journal.
    ///
    class Action:
      public elle::Object
    {
    public:
      //
      // enumerations
      //
      enum Type
	{
	  TypeUnknown,
	  TypePush,
	  TypeWipe
	};

      //
      // constructors & destructors
      //
      Action(const nucleus::Address&,
	     const nucleus::Block*);
      Action(const nucleus::Address&);

      // dumpable
      elle::Status	Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
      const Type		type;

      const nucleus::Address	address;
      const nucleus::Block*	block;
    };

  }
}

#endif
