#ifndef HOLE_IMPLEMENTATIONS_SLUG_MANIFEST_HH
# define HOLE_IMPLEMENTATIONS_SLUG_MANIFEST_HH

# include <elle/types.hh>
# include <elle/Manifest.hh>
# include <elle/network/Locus.hh>

# include <lune/fwd.hh>

# include <nucleus/fwd.hh>
# include <nucleus/proton/Version.hh>

# include <hole/implementations/slug/fwd.hh>
# include <hole/implementations/remote/Manifest.hh>

namespace hole
{
  namespace implementations
  {
    namespace slug
    {
      struct RPC: public infinit::protocol::RPC<elle::serialize::InputBinaryArchive,
                                                elle::serialize::OutputBinaryArchive>
      {
        // FIXME: voids
        RemoteProcedure<std::vector<elle::network::Locus>, lune::Passport&> authenticate;
        RemoteProcedure<bool, nucleus::proton::Address const&, nucleus::Derivable&> push;
        RemoteProcedure<nucleus::Derivable,
                        nucleus::proton::Address const&,
                        nucleus::proton::Version const&> pull;
        RemoteProcedure<bool, nucleus::proton::Address const&> wipe;
        RPC(infinit::protocol::ChanneledStream& channels);
      };
    }
  }
}


#endif
