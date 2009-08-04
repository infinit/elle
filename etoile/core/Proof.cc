//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/core/Proof.cc
//
// created       julien quintard   [mon feb 16 21:42:37 2009]
// updated       julien quintard   [tue aug  4 13:56:48 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/core/Proof.hh>

namespace etoile
{
  namespace core
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// this method initializes the object.
    ///
    Proof::Proof():
      delegate(0),
      voucher(NULL)
    {
    }

    ///
    /// this method reinitializes the object.
    ///
    Proof::~Proof()
    {
      if (this->voucher != NULL)
	delete this->voucher;
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method specifies that the proof 
    ///
    Status		Proof::Specify(Natural32		delegate)
    {
      // XXX

      leave();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps an proof object.
    ///
    Status		Proof::Dump(Natural32			margin) const
    {
      String		alignment(margin, ' ');
      String		shift(2, ' ');

      std::cout << alignment << "[Proof]" << std::endl;

      std::cout << alignment << shift << "[Delegate] "
		<< this->delegate << std::endl;

      if (this->voucher != NULL)
	{
	  if (this->voucher->Dump(margin + 2) == StatusError)
	    escape("unable to dump the voucher");
	}
      else
	{
	  std::cout << alignment << shift << "[Voucher] " << none << std::endl;
	}

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the proof object.
    ///
    Status		Proof::Serialize(Archive&		archive) const
    {
      // serialize the attributes.
      if (archive.Serialize(this->delegate) == StatusError)
	escape("unable to serialize the delegate");

      if (this->voucher != NULL)
	{
	  if (archive.Serialize(*this->voucher) == StatusError)
	    escape("unable to serialize the voucher");
	}
      else
	{
	  // serialize 'none'.
	  if (archive.Serialize(none) == StatusError)
	    escape("unable to serialize 'none'");
	}

      leave();
    }

    ///
    /// this method extracts the proof object.
    ///
    Status		Proof::Extract(Archive&			archive)
    {
      Archive::Type	type;

      // extract the attributes.
      if (archive.Extract(this->delegate) == StatusError)
	escape("unable to extract the delegate");

      // fetch the next element's type.
      if (archive.Fetch(type) == StatusError)
	escape("unable to fetch the next element's type");

      if (type == Archive::TypeNull)
	{
	  // nothing to do, keep the digest to NULL.
	  if (archive.Extract(none) == StatusError)
	    escape("unable to extract null");
	}
      else
	{
	  // allocate a digest.
	  this->voucher = new Voucher;

	  // extract the voucher.
	  if (archive.Extract(*this->voucher) == StatusError)
	    escape("unable to extract the voucher");
	}

      leave();
    }

  }
}
