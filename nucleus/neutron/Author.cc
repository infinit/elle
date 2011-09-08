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
// updated       julien quintard   [wed sep  7 18:50:25 2011]
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
    elle::Status	Author::Create(const Index&)
    {
      enter();

      // set the role.
      this->role = RoleLord;

      // XXX to complete.

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

      // XXX to complete.

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

      // XXX to complete.

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

      // XXX to complete.

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

      // XXX to complete.

      leave();
    }

  }
}
