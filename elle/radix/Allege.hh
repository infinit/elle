//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [mon jul 18 14:56:45 2011]
//

#ifndef ELLE_RADIX_ALLEGE_HH
#define ELLE_RADIX_ALLEGE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/types.hh>

namespace elle
{


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
    /// this template structure can be used to generate assertions at
    /// compile time. thus, Allege<>(_condition_) is very much equivalent
    /// to assert(_condition_) but at compile time.
    ///
    template<>
    struct Allege<true>
    {
    };

  }
}

#endif
