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
// updated       julien quintard   [fri aug 21 22:41:43 2009]
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
      // set the type.
      this->type = Subject::TypeGroup;

      // allocate the address.
      this->identifier.group = new hole::Address;

      // set the address.
      *this->identifier.group = descriptor;

      leave();
    }

  }
}
