#ifndef HOLE_IMPLEMENTATIONS_SLUG_MANIFEST_HH
# define HOLE_IMPLEMENTATIONS_SLUG_MANIFEST_HH

# include <elle/network/Locus.hh>
# include <elle/serialize/BinaryArchive.hh>

# include <lune/fwd.hh>

# include <nucleus/fwd.hh>

# include <protocol/RPC.hh>

namespace hole
{
  namespace implementations
  {
    namespace slug
    {
      struct RPC: public infinit::protocol::RPC<elle::serialize::InputBinaryArchive,
                                                elle::serialize::OutputBinaryArchive>
      {
        RemoteProcedure<std::vector<elle::network::Locus>, lune::Passport&> authenticate;
        RemoteProcedure<void, nucleus::proton::Address const&, nucleus::Derivable&> push;
        RemoteProcedure<nucleus::Derivable,
                        nucleus::proton::Address const&,
                        nucleus::proton::Revision const&> pull;
        RemoteProcedure<void, nucleus::proton::Address const&> wipe;
        RPC(infinit::protocol::ChanneledStream& channels);
      };
    }
  }
}


#endif
