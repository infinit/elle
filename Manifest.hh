//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [thu mar  4 17:35:00 2010]
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

namespace elle
{

//
// ---------- externs ---------------------------------------------------------
//

  extern const Character	Component[];

//
// ---------- constants -------------------------------------------------------
//

  ///
  /// this constant defines the number of tags reserved by Elle.
  ///
  const Natural32		Tags = 10;

}

//
// ---------- range -----------------------------------------------------------
//

///
/// allocate a range of tags for the Elle-specific messages.
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
/// as the tag is reserved for Procedure-specific purposes.
///
/// the Ok tag is however valid and can be sent and received.
///
/// Finally, the None tag is also reserved for internal use.
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
