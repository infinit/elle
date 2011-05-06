//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// file          /home/mycure/infinit/nucleus/neutron/Author.cc
//
// created       julien quintard   [fri aug 21 22:10:42 2009]
// updated       julien quintard   [wed may  4 23:12:20 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <nucleus/neutron/Author.hh>

namespace nucleus
{
  namespace neutron
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
    elle::Status	Author::Create()
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
    elle::Status	Author::Create(const Index&		index)
    {
      enter();

      // set the role.
      this->role = RoleDelegate;

      // allocate a new proof.
      this->proof = new Proof;

      // set the proof.
      if (this->proof->Specify(index) == elle::StatusError)
	escape("unable to specify the proof");

      leave();
    }

    ///
    /// this method creates a vassal-specific author object which is
    /// used whenever a user is granted access indirectly through
    /// one or more group memberships.
    ///
    elle::Status	Author::Create(const Index&		index,
				       const Voucher&		voucher)
    {
      enter();

      // set the role.
      this->role = RoleVassal;

      // allocate a new proof.
      this->proof = new Proof;

      // set the proof.
      if (this->proof->Specify(index, voucher) == elle::StatusError)
	escape("unable to specify the proof");

      leave();
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this operator compares two objects.
    ///
    elle::Boolean	Author::operator==(const Author&	element) const
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
    /// this macro-function call generates the object.
    ///
    embed(Author, _());

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps an address object.
    ///
    elle::Status	Author::Dump(elle::Natural32		margin) const
    {
      elle::String	alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Author]" << std::endl;

      std::cout << alignment << elle::Dumpable::Shift << "[Role] "
		<< (elle::Natural32)this->role << std::endl;

      if (this->proof != NULL)
	{
	  if (this->proof->Dump(margin + 2) == elle::StatusError)
	    escape("unable to dump the proof");
	}
      else
	{
	  std::cout << alignment << elle::Dumpable::Shift
		    << "[Proof] " << elle::none << std::endl;
	}

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the address object.
    ///
    elle::Status	Author::Serialize(elle::Archive&	archive) const
    {
      enter();

      // serialize the role.
      if (archive.Serialize(this->role) == elle::StatusError)
	escape("unable to serialize the role");

      if (this->proof != NULL)
	{
	  // serialize the internal proof.
	  if (archive.Serialize(*this->proof) == elle::StatusError)
	    escape("unable to serialize the proof");
	}
      else
	{
	  // serialize 'none'.
	  if (archive.Serialize(elle::none) == elle::StatusError)
	    escape("unable to serialize 'none'");
	}

      leave();
    }

    ///
    /// this method extracts the address object.
    ///
    elle::Status	Author::Extract(elle::Archive&		archive)
    {
      elle::Archive::Type	type;

      enter();

      // extract the role.
      if (archive.Extract(this->role) == elle::StatusError)
	escape("unable to extract the role");

      // fetch the next element's type.
      if (archive.Fetch(type) == elle::StatusError)
	escape("unable to fetch the next element's type");

      if (type == elle::Archive::TypeNull)
	{
	  // nothing to do, keep the proof to NULL.
	  if (archive.Extract(elle::none) == elle::StatusError)
	    escape("unable to extract null");
	}
      else
	{
	  // allocate a proof.
	  this->proof = new Proof;

	  // extract the internal proof.
	  if (archive.Extract(*this->proof) == elle::StatusError)
	    escape("unable to extract the proof");
	}

      leave();
    }

  }
}
