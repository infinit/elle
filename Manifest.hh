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
// updated       julien quintard   [wed aug 31 17:15:15 2011]
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
  const Natural32		Tags = 10;

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
      TagNone = elle::network::Range<Component>::First,

      // status
      TagOk,
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
/// note that the Error message should never be expected to be received
/// as the tag is reserved for Procedure-specific purposes. the Ok tag
/// is however valid and can be sent and received. The None tag is also
/// reserved for internal us.
///

// None
message(elle::TagNone,
	parameters());

// Ok
message(elle::TagOk,
	parameters());

// Error
message(elle::TagError,
	parameters(elle::standalone::Report));

#endif
