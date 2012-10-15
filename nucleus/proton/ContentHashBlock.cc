#include <nucleus/proton/ContentHashBlock.hh>
#include <nucleus/proton/Family.hh>
#include <nucleus/proton/Address.hh>
#include <nucleus/Exception.hh>

#include <elle/cryptography/Random.hh>
#include <elle/idiom/Open.hh>

namespace nucleus
{
  namespace proton
  {

    /*-------------.
    | Construction |
    `-------------*/

    ContentHashBlock::ContentHashBlock():
      ImmutableBlock()
    {
    }

    ContentHashBlock::ContentHashBlock(
        Network const& network,
        neutron::Component const component,
        elle::cryptography::PublicKey const& creator_K):
      ImmutableBlock(network, Family::content_hash_block, component, creator_K)
    {
    }

    /*-----------.
    | Interfaces |
    `-----------*/

    Address
    ContentHashBlock::bind() const
    {
      Address address(this->network(), this->family(), this->component(),
                      *this);

      return (address);
    }

    void
    ContentHashBlock::validate(Address const& address) const
    {
      if ((this->network() != address.network()) ||
          (this->family() != address.family()) ||
          (this->component() != address.component()))
        throw Exception("the address %s does not seem to represent the given "
                        "block", address);

      // Compute the address of this object.
      //
      // Note that compared to the other physical blocks such as PKB, OWB,
      // IB, the address of this block is computed by applying a hash on
      // its content. However, since its content contains, at least, the
      // network identifier, family and component (Block.hh), the process
      // is similar to other blocks which specifically embed these components
      // in the address.
      //
      // Indeed, the address of a CHB becomes hash(content) which happens
      // to be hash(network, family, component, ...). On the other hand,
      // the address of a PKB is computed this way: hash(network, family,
      // component, K). Therefore, all the blocks embed the network,
      // family and component in the address which helps prevent conflits.
      Address self(this->network(), this->family(), this->component(),
                   *this);

      // Compare the address with the given one.
      if (address != self)
        throw Exception("the recorded address does not correspond "
                        "to this block");
    }

    elle::Status
    ContentHashBlock::Dump(const elle::Natural32 margin) const
    {
      elle::String      alignment(margin, ' ');

      std::cout << alignment << "[ContentHashBlock]" << std::endl;

      // dump the parent class.
      if (ImmutableBlock::Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the underlying block");

      return elle::Status::Ok;
    }

    void
    ContentHashBlock::print(std::ostream& stream) const
    {
      stream << "content hash block";
    }

  }
}
