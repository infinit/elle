#include <nucleus/proton/ContentHashBlock.hh>
#include <nucleus/proton/Family.hh>
#include <nucleus/proton/Address.hh>
#include <nucleus/Exception.hh>

#include <elle/cryptography/Random.hh>
#include <elle/standalone/Report.hh>
#include <elle/idiom/Open.hh>

namespace nucleus
{
  namespace proton
  {

//
// ---------- construction ----------------------------------------------------
//

    ContentHashBlock::ContentHashBlock():
      ImmutableBlock()
    {
    }

    ContentHashBlock::ContentHashBlock(
        Network const& network,
        neutron::Component const component,
        elle::cryptography::PublicKey const& creator_K):
      ImmutableBlock(network, FamilyContentHashBlock, component, creator_K)
    {
    }

//
// ---------- block -----------------------------------------------------------
//

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

      // compute the address of this object.
      //
      // note that compared to the other physical blocks such as PKB, OWB,
      // IB, the address of this block is computed by applying a hash on
      // its content. however, since its content contains, at least, the
      // network identifier, family and component (Block.hh), the process
      // is similar to other blocks which specifically embed these components
      // in the address.
      //
      // indeed, the address of a CHB becomes hash(content) which happens
      // to be hash(network, family, component, ...). on the other hand,
      // the address of a PKB is computed this way: hash(network, family,
      // component, K). therefore, all the blocks embed the network,
      // family and component in the address which helps prevent conflits.
      Address self(this->network(), this->family(), this->component(),
                   *this);

      // compare the address with the given one.
      if (address != self)
        throw Exception("the recorded address does not correspond "
                        "to this block");
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps an block object.
    ///
    elle::Status        ContentHashBlock::Dump(const
                                                 elle::Natural32 margin) const
    {
      elle::String      alignment(margin, ' ');

      std::cout << alignment << "[ContentHashBlock]" << std::endl;

      // dump the parent class.
      if (ImmutableBlock::Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the underlying block");

      return elle::Status::Ok;
    }

  }
}
