//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// file          /home/mycure/infinit/nucleus/proton/OwnerKeyBlock.cc
//
// created       julien quintard   [fri may  6 15:34:18 2011]
// updated       julien quintard   [wed sep  7 18:48:54 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <nucleus/proton/OwnerKeyBlock.hh>
#include <nucleus/proton/Family.hh>

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
      MutableBlock()
    {
    }

    ///
    /// specific constructor.
    ///
    OwnerKeyBlock::OwnerKeyBlock(const neutron::Component	component):
      MutableBlock(FamilyOwnerKeyBlock, component)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates an OKB based on the given owner's public key.
    ///
    elle::Status	OwnerKeyBlock::Create(const elle::PublicKey& owner)
    {
      elle::KeyPair	pair;

      enter();

      // generate a key pair for the OKB.
      if (pair.Generate() == elle::StatusError)
	escape("unable to generate a key pair");

      // set the block's public key.
      this->K = pair.K;

      // set the owner's public key.
      this->owner.K = owner;

      // sign the owner's public key with the block's private key.
      if (pair.k.Sign(this->owner.K,
		      this->owner.signature) == elle::StatusError)
	escape("unable to sign the owner's identity");

      // create a subject corresponding to the user. note that this
      // subject will never be serialized hence is not really part of
      // the object but is used to ease the process of access control.
      if (this->owner._subject.Create(this->owner.K) == elle::StatusError)
	escape("unable to create the owner subject");

      leave();
    }

    ///
    /// this method computes the block's address.
    ///
    elle::Status	OwnerKeyBlock::Bind(Address&		address)
      const
    {
      enter();

      // compute the address.
      if (address.Create(this->family, this->component,
			 this->network,
			 static_cast<elle::Natural8>(this->family),
			 static_cast<elle::Natural8>(this->component),
			 this->K) == elle::StatusError)
	escape("unable to compute the OKB's address");

      leave();
    }

    ///
    /// this method verifies the block's validity.
    ///
    elle::Status	OwnerKeyBlock::Validate(const Address&	address)
      const
    {
      Address		self;

      enter();

      //
      // make sure the address has not be tampered and correspond to the
      // hash of the public key.
      //

      // compute the address.
      if (self.Create(this->family, this->component,
		      this->network,
		      static_cast<elle::Natural8>(this->family),
		      static_cast<elle::Natural8>(this->component),
		      this->K) == elle::StatusError)
	escape("unable to compute the OKB's address");

      // verify with the recorded address.
      if (address != self)
	escape("the address does not correspond to the block's public key");

      // verify the owner's key signature with the block's public key.
      if (this->K.Verify(this->owner.signature,
			 this->owner.K) == elle::StatusError)
	escape("unable to verify the owner's signature");

      leave();
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this macro-function call generates the object.
    ///
    embed(OwnerKeyBlock, _());

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps an block object.
    ///
    elle::Status	OwnerKeyBlock::Dump(const
					       elle::Natural32	margin) const
    {
      elle::String	alignment(margin, ' ');

      enter();

      std::cout << alignment << "[OwnerKeyBlock]" << std::endl;

      // dump the parent class.
      if (MutableBlock::Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the underlying block");

      // dump the OKB's public key.
      std::cout << alignment << elle::Dumpable::Shift << "[K]" << std::endl;

      if (this->K.Dump(margin + 4) == elle::StatusError)
	escape("unable to dump the public key");

      // dump the owner part.
      std::cout << alignment << elle::Dumpable::Shift << "[Owner]"
		<< std::endl;

      std::cout << alignment << elle::Dumpable::Shift << elle::Dumpable::Shift
		<< "[K]" << std::endl;

      if (this->owner.K.Dump(margin + 6) == elle::StatusError)
	escape("unable to dump the owner's public key");

      std::cout << alignment << elle::Dumpable::Shift << elle::Dumpable::Shift
		<< "[Signature]" << std::endl;

      if (this->owner.signature.Dump(margin + 6) == elle::StatusError)
	escape("unable to dump the owner's signature");

      if (this->owner._subject.Dump(margin + 6) == elle::StatusError)
	escape("unable to dump the subject");

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the block object.
    ///
    elle::Status	OwnerKeyBlock::Serialize(elle::Archive& archive) const
    {
      enter();

      // serialize the parent class.
      if (MutableBlock::Serialize(archive) == elle::StatusError)
	escape("unable to serialize the underlying block");

      // serialize the owner part.
      if (archive.Serialize(this->K,
			    this->owner.K,
			    this->owner.signature) == elle::StatusError)
	escape("unable to serialize the owner part");

      leave();
    }

    ///
    /// this method extracts the block object.
    ///
    elle::Status	OwnerKeyBlock::Extract(elle::Archive&	archive)
    {
      enter();

      // extract the parent class.
      if (MutableBlock::Extract(archive) == elle::StatusError)
	escape("unable to extract the underlying block");

      // check the family.
      if (this->family != FamilyOwnerKeyBlock)
	escape("invalid family");

      // extract the owner part.
      if (archive.Extract(this->K,
			  this->owner.K,
			  this->owner.signature) == elle::StatusError)
	escape("unable to extract the owner part");

      // compute the owner subject.
      if (this->owner._subject.Create(this->owner.K) == elle::StatusError)
	escape("unable to create the owner subject");

      leave();
    }

  }
}
