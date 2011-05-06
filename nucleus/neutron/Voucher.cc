//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// file          /home/mycure/infinit/nucleus/neutron/Voucher.cc
//
// created       julien quintard   [mon feb 16 21:42:37 2009]
// updated       julien quintard   [thu may  5 13:33:50 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <nucleus/neutron/Voucher.hh>

namespace nucleus
{
  namespace neutron
  {

//
// ---------- methods ---------------------------------------------------------
//

    // XXX

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this operator compares two objects.
    ///
    elle::Boolean	Voucher::operator==(const Voucher&	element) const
    {
      enter();

      // check the address as this may actually be the same object.
      if (this == &element)
	true();

      // compare the attributes.
      if ((this->user != element.user) ||
	  (this->signature != element.signature))
	false();

      true();
    }

    ///
    /// this macro-function call generates the object.
    ///
    embed(Voucher, _());

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps an voucher object.
    ///
    elle::Status	Voucher::Dump(elle::Natural32		margin) const
    {
      elle::String	alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Voucher]" << std::endl;

      if (this->user.Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the user key");

      if (this->signature.Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the signature");

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the voucher object.
    ///
    elle::Status	Voucher::Serialize(elle::Archive&	archive) const
    {
      enter();

      // serialize the attributes.
      if (archive.Serialize(this->user,
			    this->signature) == elle::StatusError)
	escape("unable to serialize the internal elements");

      leave();
    }

    ///
    /// this method extracts the voucher object.
    ///
    elle::Status	Voucher::Extract(elle::Archive&		archive)
    {
      enter();

      // extract the attributes.
      if (archive.Extract(this->user,
			  this->signature) == elle::StatusError)
	escape("unable to extract the internal elements");

      leave();
    }

  }
}
