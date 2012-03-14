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
      // enumerations
      //
      /* XXX[new or existing]
      enum Status
        {
          StatusLoaded,
          StatusUnloaded
        };
      */

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
      elle::Status      Load();
      elle::Status      Unload();

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
      Placement         _placement;

      Address           address;
      Block*            _block;

      Block* _XXX;
    };

  }
}

#endif
