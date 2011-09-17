//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien quintard   [mon may  2 20:10:46 2011]
//

#ifndef NUCLEUS_PROTON_PROTON_HH
#define NUCLEUS_PROTON_PROTON_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

namespace nucleus
{

  ///
  /// this namespace contains the physical elements of the file system
  /// logic.
  ///
  namespace proton
  {

    ///
    /// this class controls the proton classes.
    ///
    class Proton
    {
    public:
      //
      // static attributes
      //
      struct			Size
      {
	static elle::Natural32	Low;
	static elle::Natural32	Normal;
	static elle::Natural32	High;
      };

      //
      // static methods
      //
      static elle::Status	Initialize();
      static elle::Status	Clean();
    };

  }

}

//
// ---------- includes --------------------------------------------------------
//

#include <nucleus/proton/Address.hh>
#include <nucleus/proton/Base.hh>
#include <nucleus/proton/Block.hh>
// XXX #include <nucleus/proton/Chassis.hh>
#include <nucleus/proton/ContentHashBlock.hh>
#include <nucleus/proton/Family.hh>
#include <nucleus/proton/History.hh>
#include <nucleus/proton/ImmutableBlock.hh>
#include <nucleus/proton/ImprintBlock.hh>
#include <nucleus/proton/Location.hh>
#include <nucleus/proton/MutableBlock.hh>
#include <nucleus/proton/Network.hh>
// XXX #include <nucleus/proton/Nodule.hh>
#include <nucleus/proton/OwnerKeyBlock.hh>
// XXX #include <nucleus/proton/Porcupine.hh>
#include <nucleus/proton/PublicKeyBlock.hh>
// XXX #include <nucleus/proton/Quill.hh>
// XXX #include <nucleus/proton/Seam.hh>
#include <nucleus/proton/Stamp.hh>
#include <nucleus/proton/State.hh>
#include <nucleus/proton/Version.hh>

#endif
