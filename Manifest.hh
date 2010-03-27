//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/Manifest.hh
//
// created       julien quintard   [thu mar  4 17:35:00 2010]
// updated       julien quintard   [sat mar 27 06:08:03 2010]
//

#ifndef ELLE_MANIFEST_HH
#define ELLE_MANIFEST_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Natural.hh>
#include <elle/core/Character.hh>

#include <elle/misc/Report.hh>

#include <elle/network/Tag.hh>

#include <elle/network/Range.hh>

//
// ---------- constants -------------------------------------------------------
//

namespace elle
{

  ///
  /// XXX
  ///
  extern const Character	Component[];

  ///
  /// XXX
  ///
  const Natural32		Tags = 2;

}

//
// ---------- range -----------------------------------------------------------
//

///
/// XXX
///
range(::elle::Component, ::elle::Tags);

//
// ---------- tags ------------------------------------------------------------
//

namespace elle
{

  //
  // enumerations
  //
  enum Tag
    {
      TagNone = ::elle::network::Range<Component>::First,

      // Error
      TagError
    };

}

//
// ---------- manifests -------------------------------------------------------
//

///
/// below are the definitions of the messages common to every component
/// relying on the Elle library.
///

// Error
outward(::elle::TagError,
	parameters(const ::elle::misc::Report));

#endif
