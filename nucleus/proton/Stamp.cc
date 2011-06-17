//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// file          /home/mycure/infinit/nucleus/proton/Stamp.cc
//
// created       julien quintard   [fri jun 17 14:01:30 2011]
// updated       julien quintard   [fri jun 17 14:58:49 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <nucleus/proton/Stamp.hh>

#include <Infinit.hh>

namespace nucleus
{
  namespace proton
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method specifies the master/slave locations to stamp.
    ///
    elle::Status	Stamp::Create(const Location&		master,
				      const Location&		slave)
    {
      enter();

      // set the attributes.
      this->master = master;
      this->slave = slave;

      leave();
    }

    ///
    /// this method seals the stamp with the oracle's private key.
    ///
    elle::Status	Stamp::Seal(const elle::PrivateKey&	k)
    {
      enter();

      // sign the attributes.
      if (k.Sign(this->master, this->slave,
		 this->signature) == elle::StatusError)
	escape("unable to sign the attributes");

      leave();
    }

    ///
    /// this method verifies that the signature has been issued by the
    /// oracle.
    ///
    elle::Status	Stamp::Validate()
    {
      enter();

      // sign the attributes.
      if (Infinit::Authority.K.Verify(
	    this->signature,
	    this->master, this->slave) != elle::StatusTrue)
	flee("this stamp seems not to have been issued by the oracle");

      true();
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this operator compares two objects.
    ///
    elle::Boolean	Stamp::operator==(const Stamp&		element) const
    {
      enter();

      // check the address as this may actually be the same object.
      if (this == &element)
	true();

      // compare the attributes.
      if ((this->master != element.master) ||
	  (this->slave != element.slave) ||
	  (this->signature != element.signature))
	false();

      true();
    }

    ///
    /// this macro-function call generates the object.
    ///
    embed(Stamp, _());

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps a stamp.
    ///
    elle::Status	Stamp::Dump(elle::Natural32		margin) const
    {
      elle::String	alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Stamp]" << std::endl;

      // dump the master.

      std::cout << alignment << elle::Dumpable::Shift
		<< "[Master]" << std::endl;

      if (this->master.Dump(margin + 4) == elle::StatusError)
	escape("unable to dump the master");

      // dump the master.

      std::cout << alignment << elle::Dumpable::Shift
		<< "[Slave]" << std::endl;

      if (this->slave.Dump(margin + 4) == elle::StatusError)
	escape("unable to dump the slave");

      // dump the signature.
      if (this->signature.Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the signature");

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the stamp.
    ///
    elle::Status	Stamp::Serialize(elle::Archive&		archive) const
    {
      enter();

      // serialize the attributes.
      if (archive.Serialize(this->master,
			    this->slave,
			    this->signature) == elle::StatusError)
	escape("unable to serialize the attributes");

      leave();
    }

    ///
    /// this method extracts the stamp.
    ///
    elle::Status	Stamp::Extract(elle::Archive&		archive)
    {
      enter();

      // extract the attributes.
      if (archive.Extract(this->master,
			  this->slave,
			  this->signature) == elle::StatusError)
	escape("unable to extract the attributes");

      leave();
    }

  }
}
