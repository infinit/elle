//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien.quintard   [mon oct 10 16:27:39 2011]
//

#ifndef NUCLEUS_PROTON_HANDLE_HH
#define NUCLEUS_PROTON_HANDLE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <nucleus/proton/Address.hh>
#include <XXX/Placement.hh>

namespace nucleus
{
  namespace proton
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    class Handle:
      public elle::Object
    {
    public:
      //
      // enumerations
      //
      enum State
	{
	  StateUnloaded,
	  StateLoaded
	};

      //
      // constructors & destructors
      //
      Handle();

      // object
      declare(Handle);
      elle::Boolean	operator==(const Handle&) const;

      // dumpable
      elle::Status	Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
      State		state;

      union
      {
	Address		address;
	Placement	placement;
      };
    };

  }
}

#endif
