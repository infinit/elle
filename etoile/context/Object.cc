//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/context/Object.cc
//
// created       julien quintard   [mon aug 17 12:19:13 2009]
// updated       julien quintard   [mon aug 24 14:20:45 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/context/Object.hh>

namespace etoile
{
  namespace context
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// the constructor.
    ///
    /// note that this constructor does *not* initialize the contents.
    ///
    Object::Object():
      rights(core::PermissionNone),
      object(NULL),
      access(NULL)
    {
    }

    ///
    /// the copy constructor.
    ///
    Object::Object(const Object&				context)
    {
      // XXX \todo XXX
      printf("NOT IMPLEMENTED YET\n");
    }

    ///
    /// the destructor
    ///
    Object::~Object()
    {
      // release the object.
      if (this->object != NULL)
	delete this->object;

      // release the access.
      if (this->access != NULL)
	delete this->access;
    }

  }
}
