//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/kernel/Proof.cc
//
// created       julien quintard   [mon feb 16 21:42:37 2009]
// updated       julien quintard   [fri apr  2 11:53:30 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/kernel/Proof.hh>

namespace etoile
{
  namespace kernel
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// this method initializes the object.
    ///
    Proof::Proof():
      index(0),
      voucher(NULL)
    {
    }

    ///
    /// this method reinitializes the object.
    ///
    Proof::~Proof()
    {
      // release the voucher if present.
      if (this->voucher != NULL)
	delete this->voucher;
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method specifies a delegate-specific proof since delegates
    /// are identified by the sole index leading to the record of the object's 
    /// access control block.
    ///
    Status		Proof::Specify(const Index&		index)
    {
      enter();

      // set the index.
      this->index = index;

      // make sure there is no voucher.
      this->voucher = NULL;

      leave();
    }

    ///
    /// this method specifies a vassal-specific proof by embedding
    /// a voucher signed by a delegate or the object's owner himself.
    ///
    Status		Proof::Specify(const Index&		index,
				       const Voucher&		voucher)
    {
      enter();

      // set the index.
      this->index = index;

      // allocate and copy a new voucher.
      this->voucher = new Voucher(voucher);

      leave();
    }

//
// ---------- entity ----------------------------------------------------------
//

    ///
    /// this operator compares two objects.
    ///
    Boolean		Proof::operator==(const Proof&		element) const
    {
      enter();

      // check the address as this may actually be the same object.
      if (this == &element)
	true();

      // compare the addresses since one of them is null.
      if ((this->voucher == NULL) || (element.voucher == NULL))
	{
	  if ((this->index != element.index) ||
	      (this->voucher != element.voucher))
	    false();
	}
      else
	{
	  if ((this->index != element.index) ||
	      (*this->voucher != *element.voucher))
	    false();
	}

      true();
    }

    ///
    /// this macro-function call generates the entity.
    ///
    embed(Entity, Proof);

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps an proof object.
    ///
    Status		Proof::Dump(Natural32			margin) const
    {
      String		alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Proof]" << std::endl;

      std::cout << alignment << Dumpable::Shift << "[Index] "
		<< this->index << std::endl;

      if (this->voucher != NULL)
	{
	  if (this->voucher->Dump(margin + 2) == StatusError)
	    escape("unable to dump the voucher");
	}
      else
	{
	  std::cout << alignment << Dumpable::Shift
		    << "[Voucher] " << none << std::endl;
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
      enter();

      // serialize the attributes.
      if (archive.Serialize(this->index) == StatusError)
	escape("unable to serialize the index");

      // serialize the voucher.
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

      enter();

      // extract the attributes.
      if (archive.Extract(this->index) == StatusError)
	escape("unable to extract the index");

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
