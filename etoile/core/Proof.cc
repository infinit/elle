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
// updated       julien quintard   [wed jul 29 17:01:35 2009]
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
// ---------- definitions -----------------------------------------------------
//

    ///
    /// the class name.
    ///
    const String		Proof::Class = "Proof";

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// this method initializes the object.
    ///
    Proof::Proof()
    {
      this->voucher = NULL;
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
    Status		Proof::Dump(Natural32			margin)
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
      Archive		ar;

      // prepare the object archive.
      if (ar.Create() == StatusError)
	escape("unable to prepare the object archive");

      // serialize the class name.
      if (ar.Serialize(Proof::Class) == StatusError)
	escape("unable to serialize the class name");

      // serialize the attributes.
      if (ar.Serialize(this->delegate) == StatusError)
	escape("unable to serialize the delegate");

      if (this->voucher != NULL)
	{
	  if (ar.Serialize(*this->voucher) == StatusError)
	    escape("unable to serialize the voucher");
	}
      else
	{
	  // serialize 'none'.
	  if (ar.Serialize(none) == StatusError)
	    escape("unable to serialize 'none'");
	}

      // record in the archive.
      if (archive.Serialize(ar) == StatusError)
	escape("unable to serialize the object's archive");

      leave();
    }

    ///
    /// this method extracts the proof object.
    ///
    Status		Proof::Extract(Archive&		archive)
    {
      Archive		ar;
      String		name;
      Archive::Type	type;

      // extract the proof archive object.
      if (archive.Extract(ar) == StatusError)
	escape("unable to extract the proof archive object");

      // extract the name.
      if (ar.Extract(name) == StatusError)
	escape("unable to extract the class name");

      // check the name.
      if (Proof::Class != name)
	escape("wrong class name in the extract object");

      // extract the attributes.
      if (ar.Extract(this->delegate) == StatusError)
	escape("unable to extract the delegate");

      // fetch the next element's type.
      if (ar.Fetch(type) == StatusError)
	escape("unable to fetch the next element's type");

      if (type == Archive::TypeNull)
	{
	  // nothing to do, keep the digest to NULL.
	  if (ar.Extract(none) == StatusError)
	    escape("unable to extract null");
	}
      else
	{
	  // allocate a digest.
	  this->voucher = new Voucher;

	  // extract the voucher.
	  if (ar.Extract(*this->voucher) == StatusError)
	    escape("unable to extract the voucher");
	}

      leave();
    }

  }
}
