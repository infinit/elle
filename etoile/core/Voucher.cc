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
// updated       julien quintard   [sat aug  1 16:07:23 2009]
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
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps an voucher object.
    ///
    Status		Voucher::Dump(Natural32			margin)
    {
      String		alignment(margin, ' ');
      String		shift(2, ' ');

      std::cout << alignment << "[Voucher]" << std::endl;

      if (this->consumer.Dump(margin + 2) == StatusError)
	escape("unable to dump the consumer");

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
      // serialize the attributes.
      if (archive.Serialize(this->consumer,
			    this->signature) == StatusError)
	escape("unable to serialize the internal elements");

      leave();
    }

    ///
    /// this method extracts the voucher object.
    ///
    Status		Voucher::Extract(Archive&		archive)
    {
      // extract the attributes.
      if (archive.Extract(this->consumer,
			  this->signature) == StatusError)
	escape("unable to extract the internal elements");

      leave();
    }

  }
}
