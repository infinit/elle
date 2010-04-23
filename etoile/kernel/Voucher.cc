//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/kernel/Voucher.cc
//
// created       julien quintard   [mon feb 16 21:42:37 2009]
// updated       julien quintard   [thu mar 25 17:48:57 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/kernel/Voucher.hh>

namespace etoile
{
  namespace kernel
  {

//
// ---------- methods ---------------------------------------------------------
//

    // XXX

//
// ---------- entity ----------------------------------------------------------
//

    ///
    /// this operator compares two objects.
    ///
    Boolean		Voucher::operator==(const Voucher&	element) const
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
    /// this macro-function call generates the entity.
    ///
    embed(Entity, Voucher);

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps an voucher object.
    ///
    Status		Voucher::Dump(Natural32			margin) const
    {
      String		alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Voucher]" << std::endl;

      if (this->user.Dump(margin + 2) == StatusError)
	escape("unable to dump the user key");

      if (this->signature.Dump(margin + 2) == StatusError)
	escape("unable to dump the signature");

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the voucher object.
    ///
    Status		Voucher::Serialize(Archive&		archive) const
    {
      enter();

      // serialize the attributes.
      if (archive.Serialize(this->user,
			    this->signature) == StatusError)
	escape("unable to serialize the internal elements");

      leave();
    }

    ///
    /// this method extracts the voucher object.
    ///
    Status		Voucher::Extract(Archive&		archive)
    {
      enter();

      // extract the attributes.
      if (archive.Extract(this->user,
			  this->signature) == StatusError)
	escape("unable to extract the internal elements");

      leave();
    }

  }
}
