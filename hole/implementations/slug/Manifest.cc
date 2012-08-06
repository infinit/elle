#include <hole/implementations/slug/Manifest.hh>

namespace hole
{
  namespace implementations
  {
    namespace slug
    {
      RPC::RPC(infinit::protocol::ChanneledStream& channels)
        : infinit::protocol::RPC<elle::serialize::InputBinaryArchive,
                                 elle::serialize::OutputBinaryArchive>(channels)
        , authenticate(*this)
        , push(*this)
        , pull(*this)
        , wipe(*this)
      {}
    }
  }
}
