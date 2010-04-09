//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/kernel/Author.cc
//
// created       julien quintard   [fri aug 21 22:10:42 2009]
// updated       julien quintard   [wed apr  7 19:27:25 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/kernel/Author.hh>

namespace etoile
{
  namespace kernel
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// the constructor.
    ///
    Author::Author():
      role(RoleUnknown),
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
    /// this method creates a owner-specific author object.
    ///
    Status		Author::Create()
    {
      enter();

      // set the role.
      this->role = RoleOwner;
      this->proof = NULL;

      leave();
    }

    ///
    /// this method creates a delegate-specific author object, used whenver
    /// a user has been directly granted access to an object i.e is
    /// explicitely listed in the Access block.
    ///
    Status		Author::Create(const Index&		index)
    {
      enter();

      // set the role.
      this->role = RoleDelegate;

      // allocate a new proof.
      this->proof = new Proof;

      // set the proof.
      if (this->proof->Specify(index) == StatusError)
	escape("unable to specify the proof");

      leave();
    }

    ///
    /// this method creates a vassal-specific author object which is
    /// used whenever a user is granted access indirectly through
    /// one or more group memberships.
    ///
    Status		Author::Create(const Index&		index,
				       const Voucher&		voucher)
    {
      enter();

      // set the role.
      this->role = RoleVassal;

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
	  if ((this->role != element.role) ||
	      (this->proof != element.proof))
	    false();
	}
      else
	{
	  if ((this->role != element.role) ||
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

      enter();

      std::cout << alignment << "[Author]" << std::endl;

      std::cout << alignment << Dumpable::Shift << "[Role] "
		<< (Natural32)this->role << std::endl;

      if (this->proof != NULL)
	{
	  if (this->proof->Dump(margin + 2) == StatusError)
	    escape("unable to dump the proof");
	}
      else
	{
	  std::cout << alignment << Dumpable::Shift
		    << "[Proof] " << none << std::endl;
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

      // serialize the role.
      if (archive.Serialize(this->role) == StatusError)
	escape("unable to serialize the role");

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

      // extract the role.
      if (archive.Extract(this->role) == StatusError)
	escape("unable to extract the role");

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
