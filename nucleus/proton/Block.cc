#include <elle/cryptography/PublicKey.hh>
#include <elle/cryptography/random.hh>

#include <nucleus/proton/Block.hh>
#include <nucleus/proton/Address.hh>
#include <nucleus/Exception.hh>

#include <elle/idiom/Open.hh>

namespace nucleus
{
  namespace proton
  {
    /*----------.
    | Constants |
    `----------*/

    elle::cryptography::oneway::Algorithm const Block::Algorithms::oneway(
      elle::cryptography::oneway::Algorithm::sha256);

    /*-------------.
    | Construction |
    `-------------*/

    Block::Block():
      _component(neutron::ComponentUnknown),
      _state(StateClean)
    {
    }

    Block::Block(Network const network,
                 Family const family,
                 neutron::Component const component,
                 elle::cryptography::PublicKey const& creator_K):
      _network(network),
      _family(family),
      _component(component),
      _creator(elle::cryptography::oneway::hash(creator_K,
                                                Block::Algorithms::oneway)),
      _salt(elle::cryptography::random::generate<elle::Natural64>()),
      _state(StateClean)
    {
      // XXX[to improve and put in the list above]

      // Compute the creation timetimestamp.
      if (this->_creation_timestamp.Current() == elle::Status::Error)
        throw Exception("unable to retrieve the current time");
    }

    /*-----------.
    | Interfaces |
    `-----------*/

    elle::Status
    Block::Dump(const elle::Natural32       margin) const
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

      std::cout << alignment << elle::io::Dumpable::Shift
                << "[Creation Timestamp]" << std::endl;

      if (this->_creation_timestamp.Dump(margin + 4) == elle::Status::Error)
        escape("unable to dump the timestamp");

      std::cout << alignment << elle::io::Dumpable::Shift
                << "[Salt] " << this->_salt << std::endl;

      std::cout << alignment << elle::io::Dumpable::Shift << "[State] "
                << std::dec << this->_state << std::endl;

      return elle::Status::Ok;
    }

    void
    Block::print(std::ostream& stream) const
    {
      stream << "block{"
             << this->_network
             << ", "
             << this->_family
             << ", "
             << this->_component
             << ", "
             << this->_state
             << "}";
    }

  }
}
