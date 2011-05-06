//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// file          /home/mycure/infinit/nucleus/neutron/Subject.cc
//
// created       julien quintard   [mon aug 17 02:05:03 2009]
// updated       julien quintard   [thu may  5 15:43:28 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <nucleus/neutron/Subject.hh>

namespace nucleus
{
  namespace neutron
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
	    this->user = new elle::PublicKey(*subject.user);

	    break;
	  }
	case Subject::TypeGroup:
	  {
	    // copy the group address.
	    this->group = new proton::Address(*subject.group);

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
    elle::Status	Subject::Create(const elle::PublicKey&	K)
    {
      enter();

      // set the type.
      this->type = Subject::TypeUser;

      // allocate and copy the key.
      this->user = new elle::PublicKey(K);

      leave();
    }

    ///
    /// this method creates a group subject.
    ///
    elle::Status	Subject::Create(const proton::Address&	descriptor)
    {
      enter();

      // set the type.
      this->type = Subject::TypeGroup;

      // allocate the address.
      this->group = new proton::Address(descriptor);

      leave();
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this operator compares two objects.
    ///
    elle::Boolean	Subject::operator==(const Subject&	element) const
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
    /// this macro-function call generates the object.
    ///
    embed(Subject, _());

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps an time object.
    ///
    elle::Status	Subject::Dump(elle::Natural32		margin) const
    {
      elle::String	alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Subject]" << std::endl;

      // display the type.
      std::cout << alignment << elle::Dumpable::Shift << "[Type] "
		<< this->type << std::endl;

      // compare the identifier.
      switch (this->type)
	{
	case Subject::TypeUser:
	  {
	    std::cout << alignment << elle::Dumpable::Shift
		      << "[Identifier]" << std::endl;

	    // dump the user public key.
	    if (this->user->Dump(margin + 4) == elle::StatusError)
	      escape("unable to dump the user's public key");

	    break;
	  }
	case Subject::TypeGroup:
	  {
	    std::cout << alignment << elle::Dumpable::Shift
		      << "[Identifier]" << std::endl;

	    // dump the group address.
	    if (this->group->Dump(margin + 4) == elle::StatusError)
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
    elle::Status	Subject::Serialize(elle::Archive&	archive) const
    {
      enter();

      // serialize the type.
      if (archive.Serialize((elle::Natural8&)this->type) == elle::StatusError)
	escape("unable to serialize the type");

      // serialize the identifier.
      switch (this->type)
	{
	case Subject::TypeUser:
	  {
	    // serialize the user public key.
	    if (archive.Serialize(*this->user) == elle::StatusError)
	      escape("unable to serialize the user public key");

	    break;
	  }
	case Subject::TypeGroup:
	  {
	    // serialize the group address.
	    if (archive.Serialize(*this->group) == elle::StatusError)
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
    elle::Status	Subject::Extract(elle::Archive&		archive)
    {
      enter();

      // extract the type.
      if (archive.Extract((elle::Natural8&)this->type) == elle::StatusError)
	escape("unable to extract the type");

      // extract the identifier.
      switch (this->type)
	{
	case Subject::TypeUser:
	  {
	    // allocate a new public key.
	    this->user = new elle::PublicKey;

	    // extract the user public key.
	    if (archive.Extract(*this->user) == elle::StatusError)
	      escape("unable to extract the user public key");

	    break;
	  }
	case Subject::TypeGroup:
	  {
	    // allocate a new address.
	    this->group = new proton::Address;

	    // extract the group address.
	    if (archive.Extract(*this->group) == elle::StatusError)
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
