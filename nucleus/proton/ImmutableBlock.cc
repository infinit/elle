#include <nucleus/proton/ImmutableBlock.hh>
#include <nucleus/proton/Address.hh>
#include <nucleus/proton/Network.hh>

#include <elle/log.hh>
#include <elle/io/File.hh>
#include <elle/io/Piece.hh>

#include <lune/Lune.hh>

#include <Infinit.hh>

ELLE_LOG_COMPONENT("infinit.nucleus.proton.ImmutableBlock");

namespace nucleus
{
  namespace proton
  {

//
// ---------- construction ----------------------------------------------------
//

    ///
    /// default constructor.
    ///
    ImmutableBlock::ImmutableBlock():
      Block(FamilyUnknown, neutron::ComponentUnknown)
    {
    }

    ///
    /// specific constructor.
    ///
    ImmutableBlock::ImmutableBlock(const Family                 family,
                                   const neutron::Component     component):
      Block(family, component)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    elle::io::Path
    ImmutableBlock::_path(Network const& network,
                          Address const& address)
    {
      return (elle::io::Path(lune::Lune::Network::Shelter::ImmutableBlock,
                             elle::io::Piece("%NETWORK%", network.name),
                             elle::io::Piece("%ADDRESS%", address.unique())));
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this macro-function call generates the object.
    ///
    embed(ImmutableBlock, _());

//
// ---------- fileable --------------------------------------------------------
//

    void
    ImmutableBlock::load(Network const& network,
                         Address const& address)
    {
      this->load(ImmutableBlock::_path(network, address));
    }

    void
    ImmutableBlock::store(Network const& network,
                          Address const& address) const
    {
      this->store(ImmutableBlock::_path(network, address));
    }

    void
    ImmutableBlock::erase(Network const& network,
                          Address const& address)
    {
      elle::concept::Fileable<>::erase(ImmutableBlock::_path(network, address));
    }

    elle::Boolean
    ImmutableBlock::exists(Network const& network,
                           Address const& address)
    {
      return (elle::concept::Fileable<>::exists(
        ImmutableBlock::_path(network, address)));
    }

  }
}
