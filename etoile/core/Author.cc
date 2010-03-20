//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/core/Author.cc
//
// created       julien quintard   [fri aug 21 22:10:42 2009]
// updated       julien quintard   [sat mar 20 13:20:13 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/core/Author.hh>

namespace etoile
{
  namespace core
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// the constructor.
    ///
    Author::Author():
      mode(Author::ModeUnknown),
      proof(NULL)
    {
    }

    ///
    /// the destructor.
    ///
    Author::~Author()
    {
      // release the proof if present.
      if (this->proof != NULL)
	delete this->proof;
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    Status		Author::Create()
    {
      enter();

      // set the mode.
      this->mode = Author::ModeOwner;
      this->proof = NULL;

      leave();
    }

    ///
    /// XXX
    ///
    Status		Author::Create(const Natural32&		index)
    {
      enter();

      // set the mode.
      this->mode = Author::ModeDelegate;

      // allocate a new proof.
      this->proof = new Proof;

      // set the proof.
      if (this->proof->Specify(index) == StatusError)
	escape("unable to specify the proof");

      leave();
    }

    ///
    /// XXX
    ///
    Status		Author::Create(const Natural32&		index,
				       const Voucher&		voucher)
    {
      enter();

      // set the mode.
      this->mode = Author::ModeVassal;

      // allocate a new proof.
      this->proof = new Proof;

      // set the proof.
      if (this->proof->Specify(index, voucher) == StatusError)
	escape("unable to specify the proof");

      leave();
    }

//
// ---------- entity ----------------------------------------------------------
//

    ///
    /// this operator compares two objects.
    ///
    Boolean		Author::operator==(const Author&	element) const
    {
      enter();

      // check the address as this may actually be the same object.
      if (this == &element)
	true();

      // compare the addresses since one of them is null.
      if ((this->proof == NULL) || (element.proof == NULL))
	{
	  if ((this->mode != element.mode) ||
	      (this->proof != element.proof))
	    false();
	}
      else
	{
	  if ((this->mode != element.mode) ||
	      (*this->proof != *element.proof))
	    false();
	}

      true();
    }

    ///
    /// this macro-function call generates the entity.
    ///
    embed(Entity, Author);

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps an address object.
    ///
    Status		Author::Dump(Natural32			margin) const
    {
      String		alignment(margin, ' ');
      String		shift(2, ' ');

      enter();

      std::cout << alignment << "[Author]" << std::endl;

      std::cout << alignment << shift << "[Mode] "
		<< this->mode << std::endl;

      if (this->proof != NULL)
	{
	  if (this->proof->Dump(margin + 2) == StatusError)
	    escape("unable to dump the proof");
	}
      else
	{
	  std::cout << alignment << shift << "[Proof] " << none << std::endl;
	}

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the address object.
    ///
    Status		Author::Serialize(Archive&		archive) const
    {
      enter();

      // serialize the mode.
      if (archive.Serialize((Byte&)this->mode) == StatusError)
	escape("unable to serialize the mode");

      if (this->proof != NULL)
	{
	  // serialize the internal proof.
	  if (archive.Serialize(*this->proof) == StatusError)
	    escape("unable to serialize the proof");
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
    /// this method extracts the address object.
    ///
    Status		Author::Extract(Archive&		archive)
    {
      Archive::Type	type;

      enter();

      // extract the mode.
      if (archive.Extract((Byte&)this->mode) == StatusError)
	escape("unable to extract the mode");

      // fetch the next element's type.
      if (archive.Fetch(type) == StatusError)
	escape("unable to fetch the next element's type");

      if (type == Archive::TypeNull)
	{
	  // nothing to do, keep the proof to NULL.
	  if (archive.Extract(none) == StatusError)
	    escape("unable to extract null");
	}
      else
	{
	  // allocate a proof.
	  this->proof = new Proof;

	  // extract the internal proof.
	  if (archive.Extract(*this->proof) == StatusError)
	    escape("unable to extract the proof");
	}

      leave();
    }

  }
}
