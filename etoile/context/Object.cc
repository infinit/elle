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
// updated       julien quintard   [sat jan 30 20:54:59 2010]
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

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    Status		Object::Register(journal::Set::Container&	set)
    {
      // if there is an object.
      if (this->object != NULL)
	set.push_front(this->object);

      // if there is an access.
      if (this->access != NULL)
	set.push_front(this->access);

      leave();
    }

  }
}
