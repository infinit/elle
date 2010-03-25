//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/core/Subject.cc
//
// created       julien quintard   [mon aug 17 02:05:03 2009]
// updated       julien quintard   [thu mar 25 17:43:11 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/core/Subject.hh>

namespace etoile
{
  namespace core
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// the constructor
    ///
    Subject::Subject():
      type(Subject::TypeUnknown)
    {
    }

    ///
    /// the destructor.
    ///
    Subject::~Subject()
    {
      switch (this->type)
	{
	case Subject::TypeUser:
	  {
	    delete this->identifier.user;

	    break;
	  }
	case Subject::TypeGroup:
	  {
	    delete this->identifier.group;

	    break;
	  }
	default:
	  {
	    break;
	  }
	}
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates a user subject.
    ///
    Status		Subject::Create(const PublicKey&	K)
    {
      enter();

      // set the type.
      this->type = Subject::TypeUser;

      // allocate the key.
      this->identifier.user = new PublicKey;

      // set the key.
      *this->identifier.user = K;

      leave();
    }

    ///
    /// this method creates a group subject.
    ///
    Status		Subject::Create(const hole::Address&	descriptor)
    {
      enter();

      // set the type.
      this->type = Subject::TypeGroup;

      // allocate the address.
      this->identifier.group = new hole::Address;

      // set the address.
      *this->identifier.group = descriptor;

      leave();
    }

//
// ---------- entity ----------------------------------------------------------
//

    ///
    /// this operator compares two objects.
    ///
    Boolean		Subject::operator==(const Subject&	element) const
    {
      enter();

      // check the address as this may actually be the same object.
      if (this == &element)
	true();

      // compare the type.
      if (this->type != element.type)
	false();

      // compare the identifier.
      switch (this->type)
	{
	case Subject::TypeUser:
	  {
	    return (*this->identifier.user == *element.identifier.user);
	  }
	case Subject::TypeGroup:
	  {
	    return (*this->identifier.group == *element.identifier.group);
	  }
	case Subject::TypeUnknown:
	  {
	    true();
	  }
	}

      // unreachable.
      flee("unable to compare these objects");
    }

    ///
    /// this macro-function call generates the entity.
    ///
    embed(Entity, Subject);

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps an time object.
    ///
    Status		Subject::Dump(Natural32			margin) const
    {
      String		alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Subject]" << std::endl;

      // display the type.
      std::cout << alignment << Dumpable::Shift << "[Type] "
		<< this->type << std::endl;

      // compare the identifier.
      switch (this->type)
	{
	case Subject::TypeUser:
	  {
	    std::cout << alignment << Dumpable::Shift
		      << "[Identifier]" << std::endl;

	    // dump the user public key.
	    if (this->identifier.user->Dump(margin + 4) == StatusError)
	      escape("unable to dump the user's public key");

	    break;
	  }
	case Subject::TypeGroup:
	  {
	    std::cout << alignment << Dumpable::Shift
		      << "[Identifier]" << std::endl;

	    // dump the group address.
	    if (this->identifier.group->Dump(margin + 4) == StatusError)
	      escape("unable to dump the group address");

	    break;
	  }
	case Subject::TypeUnknown:
	  {
	    break;
	  }
	}

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the subject.
    ///
    Status		Subject::Serialize(Archive&		archive) const
    {
      enter();

      // serialize the type.
      if (archive.Serialize((Byte&)this->type) == StatusError)
	escape("unable to serialize the type");

      // serialize the identifier.
      switch (this->type)
	{
	case Subject::TypeUser:
	  {
	    // serialize the user public key.
	    if (archive.Serialize(*this->identifier.user) == StatusError)
	      escape("unable to serialize the user public key");

	    break;
	  }
	case Subject::TypeGroup:
	  {
	    // serialize the group address.
	    if (archive.Serialize(*this->identifier.group) == StatusError)
	      escape("unable to serialize the group address");

	    break;
	  }
	case Subject::TypeUnknown:
	  {
	    break;
	  }
	}

      leave();
    }

    ///
    /// this method extracts the subject.
    ///
    Status		Subject::Extract(Archive&		archive)
    {
      enter();

      // extract the type.
      if (archive.Extract((Byte&)this->type) == StatusError)
	escape("unable to extract the type");

      // extract the identifier.
      switch (this->type)
	{
	case Subject::TypeUser:
	  {
	    // allocate a new public key.
	    this->identifier.user = new PublicKey;

	    // extract the user public key.
	    if (archive.Extract(*this->identifier.user) == StatusError)
	      escape("unable to extract the user public key");

	    break;
	  }
	case Subject::TypeGroup:
	  {
	    // allocate a new address.
	    this->identifier.group = new hole::Address;

	    // extract the group address.
	    if (archive.Extract(*this->identifier.group) == StatusError)
	      escape("unable to extract the group address");

	    break;
	  }
	case Subject::TypeUnknown:
	  {
	    break;
	  }
	}

      leave();
    }

  }
}
