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
#include <nucleus/proton/Block.hh>
#include <XXX/Placement.hh>

namespace nucleus
{
  namespace proton
  {

//
// ---------- forward declarations --------------------------------------------
//

    ///
    /// this class needs forward declaration to prevent conflicts.
    ///
    template <typename V>
    class Nodule;

    ///
    /// this class needs forward declaration to prevent conflicts.
    ///
    template <typename... T>
    class Seam;

    ///
    /// this class needs forward declaration to prevent conflicts.
    ///
    template <typename... T>
    class Quill;

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
      // constants
      //
      static const Handle               Null;

      //
      // constructors & destructors
      //
      Handle();
      Handle(Block*);
      Handle(const Address&);
      Handle(const Handle&);
      ~Handle();

      //
      // methods
      //
      // XXX[rename Create into Update]
      elle::Status      Create(const Address&,
                               Block*);

      template <typename T>
      elle::Status      Use(T*&);

      //
      // interfaces
      //

      // object
      declare(Handle);
      elle::Boolean     operator==(const Handle&) const;

      // dumpable
      elle::Status      Dump(const elle::Natural32 = 0) const;

      // archivable
      elle::Status      Serialize(elle::Archive&) const;
      elle::Status      Extract(elle::Archive&);

      //
      // attributes
      //
      Address           address;

      Block*            _block;

      Placement         _placement;

      Block* _XXX;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <XXX/Handle.hxx>

#endif
