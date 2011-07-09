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
// updated       julien quintard   [tue jul  5 15:00:30 2011]
//

#ifndef ELLE_MANIFEST_HH
#define ELLE_MANIFEST_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Natural.hh>
#include <elle/core/Character.hh>

#include <elle/standalone/Report.hh>

#include <elle/radix/Parameters.hh>
#include <elle/radix/Trait.hh>

#include <elle/network/Tag.hh>
#include <elle/network/Range.hh>
#include <elle/network/Message.hh>
#include <elle/network/Bundle.hh>

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
range(elle::Component, elle::Tags);

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
      // error
      TagError = elle::network::Range<Component>::First
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
outward(elle::TagError,
	parameters(elle::standalone::Report));

/* XXX
procedure(inward(elle::TagHello,
		 parameters()),
	  outward(elle::TagWorld,
		  parameters(elle::core::String)))
*/

// XXX

message(666,
	parameters(elle::core::Natural32));
message(667,
	parameters(elle::core::Natural32));

#endif
