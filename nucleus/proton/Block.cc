#include <elle/cryptography/OneWay.hh>
#include <elle/cryptography/PublicKey.hh>

#include <nucleus/proton/Block.hh>
#include <nucleus/proton/Address.hh>
#include <nucleus/Exception.hh>

#include <elle/idiom/Open.hh>

namespace nucleus
{
  namespace proton
  {

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// this string defines the block files extension.
  ///
  const elle::String            Block::Extension = ".blk";

//
// ---------- constructs & destructors ----------------------------------------
//

    ///
    /// default constructor.
    ///
    Block::Block():
      _family(FamilyUnknown),
      _component(neutron::ComponentUnknown),
      _state(StateClean)
    {
    }

    ///
    /// specific constructor.
    ///
    Block::Block(Network const network,
                 Family const family,
                 neutron::Component const component,
                 elle::cryptography::PublicKey const& creator_K):
      _network(network),
      _family(family),
      _component(component),
      _state(StateClean)
    {
      if (elle::cryptography::OneWay::Hash(
            creator_K, this->_creator) == elle::Status::Error)
        throw Exception("unable to hash the creator's public key");
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the block's internals.
    ///
    elle::Status        Block::Dump(const elle::Natural32       margin) const
    {
      elle::String      alignment(margin, ' ');

      std::cout << alignment << "[Block]" << std::endl;

      if (this->_network.Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the network");

      std::cout << alignment << elle::io::Dumpable::Shift << "[Family] "
                << std::dec << this->_family << std::endl;

      std::cout << alignment << elle::io::Dumpable::Shift << "[Component] "
                << std::dec << this->_component << std::endl;

      if (this->_creator.Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the creator's digest");

      std::cout << alignment << elle::io::Dumpable::Shift << "[State] "
                << std::dec << this->_state << std::endl;

      return elle::Status::Ok;
    }

//
// ---------- printable -------------------------------------------------------
//

    void
    Block::print(std::ostream& stream) const
    {
      stream << "block{"
             << this->_state
             << "}";
    }

  }
}
