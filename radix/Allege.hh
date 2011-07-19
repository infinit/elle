//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/radix/Allege.hh
//
// created       julien quintard   [mon jul 18 14:56:45 2011]
// updated       julien quintard   [mon jul 18 15:02:53 2011]
//

#ifndef ELLE_RADIX_ALLEGE_HH
#define ELLE_RADIX_ALLEGE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Boolean.hh>

namespace elle
{
  using namespace core;

  namespace radix
  {

//
// ---------- structures ------------------------------------------------------
//

    ///
    /// default Allege structure.
    ///
    template<Boolean>
    struct Allege;

    ///
    /// specialized Allege class for 'true' so that the 'false'
    /// specialization remains undefined.
    ///
    template<>
    struct Allege<true>
    {
    };

  }
}

#endif
