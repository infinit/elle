//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien quintard   [fri aug 21 22:10:42 2009]
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
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this constants defines a null author.
    ///
    const Author			Author::Null;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// the constructor.
    ///
    Author::Author():
      role(RoleUnknown)
    {
    }

    ///
    /// the destructor.
    ///
    Author::~Author()
    {
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

      leave();
    }

    ///
    /// this method creates a lord-specific author object, used whenver
    /// a user has been directly granted access to an object i.e is
    /// explicitely listed in the Access block.
    ///
    elle::Status	Author::Create(const Index&		index)
    {
      enter();

      // set the role.
      this->role = RoleLord;

      // set the index.
      this->lord.index = index;

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

      // compare the role.
      if (this->role != element.role)
	false();

      // depending on the role.
      switch (this->role)
	{
	case RoleOwner:
	  {
	    //
	    // nothing more to compare.
	    //

	    break;
	  }
	case RoleLord:
	  {
	    //
	    // compare the index to the entry in the Access block.
	    //

	    // compare the indexes.
	    if (this->lord.index != element.lord.index)
	      false();

	    break;
	  }
	case RoleVassal:
	  {
	    // XXX

	    break;
	  }
	default:
	  {
	    //
	    // nothing more to compare.
	    //

	    break;
	  }
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

      // dump the role.
      std::cout << alignment << elle::Dumpable::Shift << "[Role] "
		<< this->role << std::endl;

      // depending on the role.
      switch (this->role)
	{
	case RoleOwner:
	  {
	    //
	    // nothing more to dump.
	    //

	    break;
	  }
	case RoleLord:
	  {
	    //
	    // dump the index to the entry in the Access block.
	    //

	    // dump the index.
	    std::cout << alignment << elle::Dumpable::Shift
		      << "[Index] " << std::dec << this->lord.index
		      << std::endl;

	    break;
	  }
	case RoleVassal:
	  {
	    // XXX

	    break;
	  }
	default:
	  {
	    escape("unknown role '%u'",
		   this->role);
	  }
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

      // depending on the role.
      switch (this->role)
	{
	case RoleOwner:
	  {
	    //
	    // nothing more to serialize.
	    //

	    break;
	  }
	case RoleLord:
	  {
	    //
	    // serialize the index to the entry in the Access block.
	    //

	    // serialize the index.
	    if (archive.Serialize(this->lord.index) == elle::StatusError)
	      escape("unable to serialize the index");

	    break;
	  }
	case RoleVassal:
	  {
	    // XXX

	    break;
	  }
	default:
	  {
	    escape("unknown role '%u'",
		   this->role);
	  }
	}

      leave();
    }

    ///
    /// this method extracts the address object.
    ///
    elle::Status	Author::Extract(elle::Archive&		archive)
    {
      enter();

      // extract the role.
      if (archive.Extract(this->role) == elle::StatusError)
	escape("unable to extract the role");

      // depending on the role.
      switch (this->role)
	{
	case RoleOwner:
	  {
	    //
	    // nothing more to extract.
	    //

	    break;
	  }
	case RoleLord:
	  {
	    //
	    // extract the index to the entry in the Access block.
	    //

	    // extract the index.
	    if (archive.Extract(this->lord.index) == elle::StatusError)
	      escape("unable to extract the index");

	    break;
	  }
	case RoleVassal:
	  {
	    // XXX

	    break;
	  }
	default:
	  {
	    escape("unknown role '%u'",
		   this->role);
	  }
	}

      leave();
    }

  }
}
