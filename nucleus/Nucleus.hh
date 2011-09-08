//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// file          /home/mycure/infinit/nucleus/Nucleus.hh
//
// created       julien quintard   [mon feb 16 16:46:27 2009]
// updated       julien quintard   [wed sep  7 09:08:05 2011]
//

#ifndef NUCLEUS_NUCLEUS_HH
#define NUCLEUS_NUCLEUS_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

///
/// this namespace contains the whole logic related to the Infinit
/// file system.
///
/// note that every attribute starting with an underscore, such as '_state',
/// will not be serialized, hence represent in-memory information only.
///
namespace nucleus
{

//
// ---------- classes ---------------------------------------------------------
//

  ///
  /// this class controls the nucleus library.
  ///
  class Nucleus
  {
  public:
    //
    // static methods
    //
    static elle::Status		Initialize();
    static elle::Status		Clean();

    //
    // static attributes
    //
    static elle::Factory	Factory;
  };

}

//
// ---------- includes --------------------------------------------------------
//

#include <nucleus/Derivable.hh>

#include <nucleus/neutron/Neutron.hh>
#include <nucleus/proton/Proton.hh>

//
// ---------- namespaces ------------------------------------------------------
//

namespace nucleus
{
  using namespace neutron;
  using namespace proton;
}

#endif
