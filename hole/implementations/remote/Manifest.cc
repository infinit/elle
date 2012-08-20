#include <hole/implementations/remote/Manifest.hh>

namespace hole
{
  namespace implementations
  {
    namespace remote
    {
      RPC::RPC(infinit::protocol::ChanneledStream& channels)
        : infinit::protocol::RPC<elle::serialize::InputBinaryArchive,
                                 elle::serialize::OutputBinaryArchive>(channels)
        , challenge(*this)
        , push(*this)
        , pull(*this)
        , wipe(*this)
      {}
    }
  }
}
