#include <cryptography/PublicKey.hh>
#include <cryptography/random.hh>

#include <nucleus/proton/Block.hh>
#include <nucleus/proton/Address.hh>
#include <nucleus/Exception.hh>

namespace nucleus
{
  namespace proton
  {
    /*----------.
    | Constants |
    `----------*/

    cryptography::oneway::Algorithm const Block::Constants::oneway_algorithm(
      cryptography::oneway::Algorithm::sha256);

    /*-------------.
    | Construction |
    `-------------*/

    Block::Block():
      _component(neutron::ComponentUnknown),
      _state(State::clean)
    {
    }

    Block::Block(Network const network,
                 Family const family,
                 neutron::Component const component,
                 cryptography::PublicKey const& creator_K):
      _network(network),
      _family(family),
      _component(component),
      _creator(cryptography::oneway::hash(creator_K,
                                          Block::Constants::oneway_algorithm)),
      _creation_timestamp(elle::utility::Time::current()),
      _salt(cryptography::random::generate<elle::Natural64>()),
      _state(State::clean)
    {
    }

    ELLE_SERIALIZE_CONSTRUCT_DEFINE(Block)
    {
    }

    Block::~Block()
    {
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
        throw Exception("unable to dump the network");

      std::cout << alignment << elle::io::Dumpable::Shift << "[Family] "
                << std::dec << this->_family << std::endl;

      std::cout << alignment << elle::io::Dumpable::Shift << "[Component] "
                << std::dec << this->_component << std::endl;

      std::cout << alignment << elle::io::Dumpable::Shift << "[Creator K] "
                << this->_creator << std::endl;

      std::cout << alignment << elle::io::Dumpable::Shift
                << "[Creation Timestamp]" << std::endl;

      if (this->_creation_timestamp.Dump(margin + 4) == elle::Status::Error)
        throw Exception("unable to dump the timestamp");

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
