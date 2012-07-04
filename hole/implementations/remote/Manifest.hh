#ifndef HOLE_IMPLEMENTATIONS_REMOTE_MANIFEST_HH
# define HOLE_IMPLEMENTATIONS_REMOTE_MANIFEST_HH

# include <elle/types.hh>

# include <nucleus/fwd.hh>

# include <lune/fwd.hh>
# include <nucleus/Nucleus.hh>
# include <nucleus/Derivable.hh>

# include <etoile/Manifest.hh>

# include <protocol/RPC.hh>

namespace hole
{
  namespace implementations
  {
    namespace remote
    {
      struct RPC: public infinit::protocol::RPC<elle::serialize::InputBinaryArchive,
                                                elle::serialize::OutputBinaryArchive>
      {
        RemoteProcedure<bool, lune::Passport&> challenge;
        RemoteProcedure<bool> authenticated;
        RemoteProcedure<bool,
                        nucleus::proton::Address const&,
                        nucleus::Derivable&> push;
        RemoteProcedure<nucleus::Derivable,
                        nucleus::proton::Address const&,
                        nucleus::proton::Version const&> pull;
        RemoteProcedure<bool, nucleus::Derivable&> block;
        RemoteProcedure<bool, nucleus::proton::Address const&> wipe;

        RPC(infinit::protocol::ChanneledStream& channels)
          : infinit::protocol::RPC<elle::serialize::InputBinaryArchive,
                                   elle::serialize::OutputBinaryArchive>(channels)
          , challenge(*this)
          , authenticated(*this)
          , push(*this)
          , pull(*this)
          , block(*this)
          , wipe(*this)
        {}
      };
    }
  }
}

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
          TagChallenge = elle::network::Range<Component>::First + 1,
          TagAuthenticated,

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

message(hole::implementations::remote::TagChallenge,
        parameters(lune::Passport&));
message(hole::implementations::remote::TagAuthenticated,
        parameters());

message(hole::implementations::remote::TagPush,
        parameters(nucleus::proton::Address&, nucleus::Derivable&));
message(hole::implementations::remote::TagPull,
        parameters(nucleus::proton::Address&,
                   nucleus::proton::Version&));
message(hole::implementations::remote::TagBlock,
        parameters(nucleus::Derivable&));
message(hole::implementations::remote::TagWipe,
        parameters(nucleus::proton::Address&));

#endif
