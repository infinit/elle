//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/context/Object.hh
//
// created       julien quintard   [fri aug 14 23:13:51 2009]
// updated       julien quintard   [sun aug 23 15:32:41 2009]
//

#ifndef ETOILE_CONTEXT_OBJECT_HH
#define ETOILE_CONTEXT_OBJECT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/core/Core.hh>
#include <etoile/hole/Hole.hh>

namespace etoile
{
  namespace context
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    /// when the object is loaded or created, the permissions on the
    /// object for the subject are recorded in the permissions field.
    ///
    class Object
    {
    public:
      //
      // constructors & destructors
      Object();
      Object(const Object&);
      ~Object();

      //
      // attributes
      //
      // XXX path::Route		route;
      hole::Address			address;

      core::Object*			object;
      core::Access*			access;

      core::Author			author;
      core::Permissions			rights;
    };

  }
}

#endif
