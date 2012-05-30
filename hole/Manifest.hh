//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// author        julien quintard   [mon oct 31 16:20:39 2011]
//

#ifndef HOLE_MANIFEST_HH
#define HOLE_MANIFEST_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/types.hh>

#include <hole/implementations/slug/Manifest.hh>

//
// ---------- constants -------------------------------------------------------
//

namespace hole
{

  ///
  /// the component name.
  ///
  extern const elle::Character  Component[];

  ///
  /// this constants defines the number of tags to reserve for hole.
  ///
  const elle::Natural32         Tags = 0;

}

//
// ---------- range -----------------------------------------------------------
//

///
/// this macro-function calls reserves a range of tags.
///
range(hole::Component,
      hole::Tags,
      hole::implementations::slug::Component);

#endif
