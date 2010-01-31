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
// updated       julien quintard   [sat jan 30 22:27:56 2010]
//

//
// ---------- includes --------------------------------------------------------
//

///
/// this include has been placed here in order to avoid a pre-processing
/// looping issue
///
#include <etoile/context/Context.hh>

#ifndef ETOILE_CONTEXT_OBJECT_HH
#define ETOILE_CONTEXT_OBJECT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/core/Core.hh>
#include <etoile/hole/Hole.hh>

#include <list>

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
    class Object:
      public Context
    {
    public:
      //
      // constructors & destructors
      Object();
      Object(const Object&);
      ~Object();

      //
      // methods
      //
      Status		Register(journal::Set::Container&);

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
