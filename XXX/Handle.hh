//
// ---------- header ----------------------------------------------------------
//
// project       XXX
//
// license       infinit
//
// author        julien quintard   [sun dec 25 14:05:57 2011]
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
    template <typename T>
    class Handle:
      public elle::Object
    {
    public:
      //
      // constants
      //
      static const Handle		Null;

      //
      // constructors & destructors
      //
      Handle();
      Handle(T*);
      Handle(const Address);
      ~Handle();

      //
      // methods
      //
      elle::Status	Create(const Address&,
			       T*);

      //
      // attributes
      //
      Address		address;

      T*		_object;
      Placement		_placement;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <XXX/Handle.hxx>

#endif
