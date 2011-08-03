//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// file          /home/mycure/infinit/nucleus/proton/ImprintBlock.cc
//
// created       julien quintard   [sat may  7 23:41:32 2011]
// updated       julien quintard   [mon aug  1 13:29:08 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <nucleus/proton/ImprintBlock.hh>
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
    ImprintBlock::ImprintBlock():
      MutableBlock()
    {
    }

    ///
    /// specific constructor.
    ///
    ImprintBlock::ImprintBlock(const neutron::Component		component):
      MutableBlock(FamilyImprintBlock, component)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates an imprint based on the given owner's public key.
    ///
    elle::Status	ImprintBlock::Create(const elle::PublicKey& owner)
    {
      enter();

      // retrieve the current time.
      if (this->seed.stamp.Current() == elle::StatusError)
	escape("unable to retrieve the current time");

      // generate a random number.
      this->seed.salt = (elle::Natural64)::rand();

      // set the owner public key.
      this->owner.K = owner;

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
    elle::Status	ImprintBlock::Bind(Address&		address)
      const
    {
      enter();

      // compute the address.
      if (address.Create(this->family, this->component,
			 this->network,
			 (elle::Natural8&)this->family,
			 (elle::Natural8&)this->component,
			 this->seed.stamp, this->seed.salt, this->owner.K) ==
	  elle::StatusError)
	escape("unable to compute the imprint address");

      leave();
    }

    ///
    /// this method verifies the block's validity.
    ///
    elle::Status	ImprintBlock::Validate(const Address&	address)
      const
    {
      Address		self;

      enter();

      //
      // make sure the address has not be tampered and correspond to the
      // hash of the tuple (stamp, salt, owner public key).
      //

      // compute the address.
      if (self.Create(this->family, this->component,
		      this->network,
		      (elle::Natural8&)this->family,
		      (elle::Natural8&)this->component,
		      this->seed.stamp, this->seed.salt, this->owner.K) ==
	  elle::StatusError)
	escape("unable to compute the imprint address");

      // verify with the recorded address.
      if (address != self)
	escape("the address does not correspond to the block's public key");

      leave();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps an block object.
    ///
    elle::Status	ImprintBlock::Dump(const
					     elle::Natural32	margin) const
    {
      elle::String	alignment(margin, ' ');

      enter();

      std::cout << alignment << "[ImprintBlock]" << std::endl;

      // dump the parent class.
      if (MutableBlock::Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the underlying block");

      // dump the seed.
      std::cout << alignment << elle::Dumpable::Shift
		<< "[Seed]" << std::endl;

      // dump the stamp.
      std::cout << alignment << elle::Dumpable::Shift << elle::Dumpable::Shift
		<< "[Stamp]" << std::endl;

      if (this->seed.stamp.Dump(margin + 6) == elle::StatusError)
	escape("unable to dump the stamp");

      // dump the salt.
      std::cout << alignment << elle::Dumpable::Shift << elle::Dumpable::Shift
		<< "[Salt] " << this->seed.salt << std::endl;

      // dump the owner.
      std::cout << alignment << elle::Dumpable::Shift
		<< "[Owner]" << std::endl;

      // dump the owner's public key.
      if (this->owner.K.Dump(margin + 4) == elle::StatusError)
	escape("unable to dump the owner's public key");

      // dump the subject.
      if (this->owner._subject.Dump(margin + 4) == elle::StatusError)
	escape("unable to dump the subject");

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the block object.
    ///
    elle::Status	ImprintBlock::Serialize(elle::Archive& archive) const
    {
      enter();

      // serialize the parent class.
      if (MutableBlock::Serialize(archive) == elle::StatusError)
	escape("unable to serialize the underlying block");

      // serialize the owner part.
      if (archive.Serialize(this->seed.stamp,
			    this->seed.salt,
			    this->owner.K) == elle::StatusError)
	escape("unable to serialize the block's content");

      leave();
    }

    ///
    /// this method extracts the block object.
    ///
    elle::Status	ImprintBlock::Extract(elle::Archive&	archive)
    {
      enter();

      // extract the parent class.
      if (MutableBlock::Extract(archive) == elle::StatusError)
	escape("unable to extract the underlying block");

      // check the family.
      if (this->family != FamilyImprintBlock)
	escape("invalid family");

      // extract the owner part.
      if (archive.Extract(this->seed.stamp,
			  this->seed.salt,
			  this->owner.K) == elle::StatusError)
	escape("unable to extract the block's content");

      // compute the owner subject.
      if (this->owner._subject.Create(this->owner.K) == elle::StatusError)
	escape("unable to create the owner subject");

      leave();
    }

  }
}
