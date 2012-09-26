#include <nucleus/proton/OwnerKeyBlock.hh>
#include <nucleus/proton/Family.hh>
#include <nucleus/proton/Network.hh>
#include <nucleus/proton/Address.hh>

#include <elle/cryptography/KeyPair.hh>

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
    OwnerKeyBlock::OwnerKeyBlock():
      MutableBlock(),

      _owner_subject(nullptr)
    {
    }

    ///
    /// specific constructor.
    ///
    OwnerKeyBlock::OwnerKeyBlock(
        Network const& network,
        neutron::Component const component,
        elle::cryptography::PublicKey const& creator_K):
      MutableBlock(network, FamilyOwnerKeyBlock, component, creator_K)
    {
      elle::cryptography::KeyPair pair;

      // retrieve the current time.
      if (this->_stamp.Current() == elle::Status::Error)
        throw Exception("unable to retrieve the current time");

      // generate a key pair for the OKB.
      if (pair.Generate() == elle::Status::Error)
        throw Exception("unable to generate a key pair");

      // set the block's public key.
      this->_K = pair.K;

      // set the owner's public key.
      this->_owner_K = creator_K; // XXX[?]
      this->_owner_subject = nullptr;

      // sign the owner's public key with the block's private key.
      if (pair.k.Sign(this->_owner_K,
                      this->_owner_signature) == elle::Status::Error)
        throw Exception("unable to sign the owner's identity");
    }

//
// ---------- methods ---------------------------------------------------------
//

    Address
    OwnerKeyBlock::bind() const
    {
      Address address;

      if (address.Create(this->network(), this->family(), this->component(),
                         this->_K) == elle::Status::Error)
        throw Exception("unable to compute the OKB's address");

      return (address);
    }

    void
    OwnerKeyBlock::validate(Address const& address) const
    {
      Address self;

      if ((this->network() != address.network()) ||
          (this->family() != address.family()) ||
          (this->component() != address.component()))
        throw Exception(
          elle::sprint("the address %s does not seem to represent the given "
                       "block", address));

      //
      // make sure the address has not be tampered and correspond to the
      // hash of the public key.
      //

      // compute the address.
      if (self.Create(this->network(), this->family(), this->component(),
                      this->_K) == elle::Status::Error)
        throw Exception("unable to compute the OKB's address");

      // verify with the recorded address.
      if (address != self)
        throw Exception("the address does not correspond to the block's "
                        "public key");

      // verify the owner's key signature with the block's public key.
      if (this->_K.Verify(this->_owner_signature,
                          this->_owner_K) == elle::Status::Error)
        throw Exception("unable to verify the owner's signature");
    }

    neutron::Subject const&
    OwnerKeyBlock::owner_subject()
    {
      // Create the subject corresponding to the block owner, if necessary.
      // Note that this subject will never be serialized but is used to ease
      // the process of access control since most method require a subject.
      if (this->_owner_subject == nullptr)
        this->_owner_subject = new neutron::Subject(this->_owner_K);

      assert(this->_owner_subject != nullptr);

      return (*this->_owner_subject);
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps an block object.
    ///
    elle::Status        OwnerKeyBlock::Dump(const
                                               elle::Natural32  margin) const
    {
      elle::String      alignment(margin, ' ');

      std::cout << alignment << "[OwnerKeyBlock]" << std::endl;

      // dump the parent class.
      if (MutableBlock::Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the underlying block");

      // dump the OKB's public key.
      std::cout << alignment << elle::io::Dumpable::Shift
                << "[K]" << std::endl;

      if (this->_K.Dump(margin + 4) == elle::Status::Error)
        escape("unable to dump the public key");

      // dump the stamp.
      std::cout << alignment << elle::io::Dumpable::Shift
                << elle::io::Dumpable::Shift
                << "[Stamp]" << std::endl;

      if (this->_stamp.Dump(margin + 6) == elle::Status::Error)
        escape("unable to dump the stamp");

      // dump the owner part.
      std::cout << alignment << elle::io::Dumpable::Shift << "[Owner]"
                << std::endl;

      std::cout << alignment << elle::io::Dumpable::Shift
                << elle::io::Dumpable::Shift
                << "[K]" << std::endl;

      if (this->_owner_K.Dump(margin + 6) == elle::Status::Error)
        escape("unable to dump the owner's public key");

      std::cout << alignment << elle::io::Dumpable::Shift
                << elle::io::Dumpable::Shift
                << "[Signature]" << std::endl;

      if (this->_owner_signature.Dump(margin + 6) == elle::Status::Error)
        escape("unable to dump the owner's signature");

      if (this->_owner_subject->Dump(margin + 6) == elle::Status::Error)
        escape("unable to dump the subject");

      return elle::Status::Ok;
    }

  }
}
