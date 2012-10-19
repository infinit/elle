#include <nucleus/proton/ImmutableBlock.hh>
#include <nucleus/proton/Address.hh>
#include <nucleus/proton/Network.hh>

#include <elle/io/File.hh>
#include <elle/io/Piece.hh>

#include <Infinit.hh>

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
        elle::cryptography::PublicKey const& creator_K):
      Block(network, family, component, creator_K)
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
