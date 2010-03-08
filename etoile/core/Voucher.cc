//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/core/Voucher.cc
//
// created       julien quintard   [mon feb 16 21:42:37 2009]
// updated       julien quintard   [wed mar  3 16:20:44 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/core/Voucher.hh>

namespace etoile
{
  namespace core
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
      return ((this->user == element.user) &&
	      (this->signature == element.signature));
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps an voucher object.
    ///
    Status		Voucher::Dump(Natural32			margin) const
    {
      String		alignment(margin, ' ');
      String		shift(2, ' ');

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
