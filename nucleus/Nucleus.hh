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
// updated       julien quintard   [fri jun 10 13:08:43 2011]
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
namespace nucleus
{

//
// ---------- classes ---------------------------------------------------------
//

  ///
  /// XXX
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
