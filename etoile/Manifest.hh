//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien quintard   [mon oct 31 16:11:03 2011]
//

#ifndef ETOILE_MANIFEST_HH
#define ETOILE_MANIFEST_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/portal/Manifest.hh>

//
// ---------- constants -------------------------------------------------------
//

namespace etoile
{

  ///
  /// the component name.
  ///
  extern const elle::Character	Component[];

  ///
  /// this constants defines the number of tags to reserve for
  /// this implementation.
  ///
  const elle::Natural32		Tags = 0;

}

//
// ---------- range -----------------------------------------------------------
//

///
/// this macro-function call reserves a range of tags.
///
range(etoile::Component,
      etoile::Tags,
      etoile::portal::Component);

#endif
