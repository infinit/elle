//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// author        julien quintard   [thu may 26 12:59:43 2011]
//

#ifndef HOLE_IMPLEMENTATIONS_REMOTE_MANIFEST_HH
#define HOLE_IMPLEMENTATIONS_REMOTE_MANIFEST_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/types.hh>
#include <nucleus/Nucleus.hh>
#include <lune/Lune.hh>

#include <etoile/Manifest.hh>

//
// ---------- constants -------------------------------------------------------
//

namespace hole
{
  namespace implementations
  {
    namespace remote
    {

      ///
      /// the component name.
      ///
      extern const elle::Character      Component[];

      ///
      /// this constants defines the number of tags to reserve for
      /// this implementation.
      ///
      const elle::Natural32             Tags = 20;

    }
  }
}

//
// ---------- range -----------------------------------------------------------
//

///
/// this macro-function calls reserves a range of tags.
///
range(hole::implementations::remote::Component,
      hole::implementations::remote::Tags,
      etoile::Component);

//
// ---------- tags ------------------------------------------------------------
//

namespace hole
{
  namespace implementations
  {
    namespace remote
    {

      //
      // enumerations
      //
      enum Tag
        {
          TagChallenge = elle::Range<Component>::First + 1,
          TagAuthenticated,

          TagPush,
          TagPull,
          TagBlock,
          TagWipe,

          TagException
        };

    }
  }
}

//
// ---------- manifests -------------------------------------------------------
//

///
/// below are the definitions of the inward and outward messages.
///

message(hole::implementations::remote::TagChallenge,
        parameters(lune::Passport&));
message(hole::implementations::remote::TagAuthenticated,
        parameters());

message(hole::implementations::remote::TagPush,
        parameters(nucleus::Address&,
                   nucleus::Derivable<nucleus::Block>&));
message(hole::implementations::remote::TagPull,
        parameters(nucleus::Address&,
                   nucleus::Version&));
message(hole::implementations::remote::TagBlock,
        parameters(nucleus::Derivable<nucleus::Block>&));
message(hole::implementations::remote::TagWipe,
        parameters(nucleus::Address&));

message(hole::implementations::remote::TagException,
        parameters(elle::Report&));

#endif
