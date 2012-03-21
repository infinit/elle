//
// ---------- header ----------------------------------------------------------
//
// project       XXX
//
// license       infinit
//
// author        julien quintard   [mon mar 12 19:44:52 2012]
//

#ifndef NUCLEUS_PROTON_AMBIT_HH
#define NUCLEUS_PROTON_AMBIT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <nucleus/proton/Block.hh>
#include <XXX/Handle.hh>

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
    class Ambit:
      public elle::Entity
    {
    public:
      //
      // enumerations
      //
      enum Mode
        {
          ModeManual,
          ModeAutomatic,
          ModeTransparent
        };

      //
      // constructors & destructors
      //
      Ambit(Handle&,
            Mode = ModeAutomatic);
      ~Ambit();

      //
      // methods
      //
      elle::Status      Load();
      elle::Status      Unload();

      //
      // accessors
      //
      T*                contents();

      typename T::C*    nodule();
      typename T::C*    content();

      //
      // interfaces
      //

      // dumpable
      elle::Status      Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
      Mode              mode;

      Handle&           handle;
    };

  }
}

#include <XXX/Ambit.hxx>

#endif
