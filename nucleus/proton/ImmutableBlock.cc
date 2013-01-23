#include <nucleus/proton/ImmutableBlock.hh>
#include <nucleus/proton/Address.hh>
#include <nucleus/proton/Network.hh>

#include <elle/io/File.hh>
#include <elle/io/Piece.hh>

namespace nucleus
{
  namespace proton
  {
    /*-------------.
    | Construction |
    `-------------*/

    ImmutableBlock::ImmutableBlock():
      Block()
    {
    }

    ImmutableBlock::ImmutableBlock(
        Network const& network,
        Family const family,
        neutron::Component const component,
        cryptography::PublicKey const& creator_K):
      Block(network, family, component, creator_K)
    {
    }

    ELLE_SERIALIZE_CONSTRUCT_DEFINE(ImmutableBlock, Block)
    {
    }

    /*----------.
    | Printable |
    `----------*/

    void
    ImmutableBlock::print(std::ostream& stream) const
    {
      stream << "immutable block";
    }
  }
}
