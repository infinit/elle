#ifndef HOLE_IMPLEMENTATIONS_REMOTE_MANIFEST_HH
# define HOLE_IMPLEMENTATIONS_REMOTE_MANIFEST_HH

# include <elle/fwd.hh>
# include <elle/types.hh>

# include <nucleus/fwd.hh>

# include <nucleus/Derivable.hh>

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
        RemoteProcedure<bool, elle::Passport const&> challenge;
        RemoteProcedure<void,
                        nucleus::proton::Address const&,
                        nucleus::Derivable&> push;
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
