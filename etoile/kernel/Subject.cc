//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/kernel/Subject.cc
//
// created       julien quintard   [mon aug 17 02:05:03 2009]
// updated       julien quintard   [tue apr  6 22:45:34 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/kernel/Subject.hh>

namespace etoile
{
  namespace kernel
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
    /// copy constructor.
    ///
    Subject::Subject(const Subject&				subject)
    {
      // set the type.
      this->type = subject.type;

      switch (this->type)
	{
	case Subject::TypeUser:
	  {
	    // copy the user public key.
	    this->user = new PublicKey(*subject.user);

	    break;
	  }
	case Subject::TypeGroup:
	  {
	    // copy the group address.
	    this->group = new hole::Address(*subject.group);

	    break;
	  }
	default:
	  {
	    break;
	  }
	}
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
	    delete this->user;

	    break;
	  }
	case Subject::TypeGroup:
	  {
	    delete this->group;

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

      // allocate and copy the key.
      this->user = new PublicKey(K);

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
      this->group = new hole::Address(descriptor);

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
	    return (*this->user == *element.user);
	  }
	case Subject::TypeGroup:
	  {
	    return (*this->group == *element.group);
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
	    if (this->user->Dump(margin + 4) == StatusError)
	      escape("unable to dump the user's public key");

	    break;
	  }
	case Subject::TypeGroup:
	  {
	    std::cout << alignment << Dumpable::Shift
		      << "[Identifier]" << std::endl;

	    // dump the group address.
	    if (this->group->Dump(margin + 4) == StatusError)
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
      if (archive.Serialize((Natural8&)this->type) == StatusError)
	escape("unable to serialize the type");

      // serialize the identifier.
      switch (this->type)
	{
	case Subject::TypeUser:
	  {
	    // serialize the user public key.
	    if (archive.Serialize(*this->user) == StatusError)
	      escape("unable to serialize the user public key");

	    break;
	  }
	case Subject::TypeGroup:
	  {
	    // serialize the group address.
	    if (archive.Serialize(*this->group) == StatusError)
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
      if (archive.Extract((Natural8&)this->type) == StatusError)
	escape("unable to extract the type");

      // extract the identifier.
      switch (this->type)
	{
	case Subject::TypeUser:
	  {
	    // allocate a new public key.
	    this->user = new PublicKey;

	    // extract the user public key.
	    if (archive.Extract(*this->user) == StatusError)
	      escape("unable to extract the user public key");

	    break;
	  }
	case Subject::TypeGroup:
	  {
	    // allocate a new address.
	    this->group = new hole::Address;

	    // extract the group address.
	    if (archive.Extract(*this->group) == StatusError)
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
