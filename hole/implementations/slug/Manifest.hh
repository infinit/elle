#ifndef HOLE_IMPLEMENTATIONS_SLUG_MANIFEST_HH
# define HOLE_IMPLEMENTATIONS_SLUG_MANIFEST_HH

# include <elle/types.hh>
# include <elle/Manifest.hh>

# include <lune/fwd.hh>

# include <nucleus/fwd.hh>

# include <hole/implementations/slug/fwd.hh>
# include <hole/implementations/remote/Manifest.hh>

//
// ---------- constants -------------------------------------------------------
//

namespace hole
{
  namespace implementations
  {
    namespace slug
    {

      ///
      /// XXX
      ///
      extern const elle::Character      Component[];

      ///
      /// XXX
      ///
      const elle::Natural32             Tags = 30;

    }
  }
}

//
// ---------- range -----------------------------------------------------------
//

///
/// XXX
///
range(hole::implementations::slug::Component,
      hole::implementations::slug::Tags,
      hole::implementations::remote::Component);

//
// ---------- tags ------------------------------------------------------------
//

namespace hole
{
  namespace implementations
  {
    namespace slug
    {

      //
      // enumerations
      //
      enum Tag
        {
          TagAuthenticate = elle::network::Range<Component>::First + 1,
          TagAuthenticated,

          TagSynchronise,
          TagBank,

          TagPush,
          TagPull,
          TagBlock,
          TagWipe
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

message(hole::implementations::slug::TagAuthenticate,
        parameters(lune::Passport&,
                   elle::network::Port&))
message(hole::implementations::slug::TagAuthenticated,
        parameters(hole::implementations::slug::Cluster&))

message(hole::implementations::slug::TagSynchronise,
        parameters())
message(hole::implementations::slug::TagBank,
        parameters(hole::implementations::slug::Bank&))

message(hole::implementations::slug::TagPush,
        parameters(nucleus::proton::Address&,
                   nucleus::Derivable&));
message(hole::implementations::slug::TagPull,
        parameters(nucleus::proton::Address&,
                   nucleus::proton::Version&));
message(hole::implementations::slug::TagBlock,
        parameters(nucleus::Derivable&));
message(hole::implementations::slug::TagWipe,
        parameters(nucleus::proton::Address&));

#endif
