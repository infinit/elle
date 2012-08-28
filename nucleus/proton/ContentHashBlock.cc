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
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    ContentHashBlock::ContentHashBlock():
      ImmutableBlock()
    {
      // Compute the creation timestamp.
      if (this->_creation_stamp.Current() == elle::Status::Error)
        throw Exception("unable to retrieve the current time");

      // Generate a random number for the salt.
      if (elle::cryptography::Random::Generate(this->_salt) == elle::Status::Error)
        throw Exception("unable to generate the seed");
    }

    ///
    /// specific constructor.
    ///
    ContentHashBlock::ContentHashBlock(const neutron::Component component):
      ImmutableBlock(FamilyContentHashBlock, component)
    {
      // Compute the creation timestamp.
      if (this->_creation_stamp.Current() == elle::Status::Error)
        throw Exception("unable to retrieve the current time");

      // Generate a random number for the salt.
      if (elle::cryptography::Random::Generate(this->_salt) == elle::Status::Error)
        throw Exception("unable to generate the seed");
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method computes the address of the block.
    ///
    elle::Status        ContentHashBlock::Bind(Address&         address)
      const
    {
      // compute the address.
      if (address.Create(
            this->family, this->component,
            elle::serialize::serializable(*this)) == elle::Status::Error)
        escape("unable to compute the CHB's address");

      return elle::Status::Ok;
    }

    ///
    /// this method verifies that the block is valid according to the
    /// given requested address.
    ///
    elle::Status        ContentHashBlock::Validate(const Address& address)
      const
    {
      Address           self;

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
      if (self.Create(this->family, this->component,
                      elle::serialize::serializable(*this)) == elle::Status::Error)
        escape("unable to compute the CHB's address");

      // compare the address with the given one.
      if (address != self)
        escape("the recorded address does not correspond to this block");

      return elle::Status::Ok;
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this macro-function call generates the object.
    ///
    embed(ContentHashBlock, _());

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

      std::cout << alignment << elle::io::Dumpable::Shift
                << "[Salt] " << this->_salt << std::endl;

      return elle::Status::Ok;
    }

  }
}
