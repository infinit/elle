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
// updated       julien quintard   [mon aug 17 12:30:23 2009]
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
      dirty(Object::DirtyNone),
      rights(core::PermissionNone),
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
      // XXX \todo XXX
    }

  }
}
