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
      static const Handle               Null;

      //
      // constructors & destructors
      //
      Handle();
      Handle(T*);
      Handle(const Address&);
      Handle(const Handle<T>&);
      ~Handle();

      //
      // methods
      //
      // XXX[rename Create into Update]
      elle::Status      Create(const Address&,
                               T*);

      //
      // interfaces
      //

      // object
      declare(Handle<T>);
      elle::Boolean     operator==(const Handle<T>&) const;

      // dumpable
      elle::Status      Dump(const elle::Natural32 = 0) const;

      // archivable
      elle::Status      Serialize(elle::Archive&) const;
      elle::Status      Extract(elle::Archive&);

      //
      // attributes
      //
      Address           address;

      T*                _object;
      Placement         _placement;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <XXX/Handle.hxx>

#endif
