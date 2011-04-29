//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/context/Object.hh
//
// created       julien quintard   [fri aug 14 23:13:51 2009]
// updated       julien quintard   [thu apr 28 18:32:56 2011]
//

#ifndef ETOILE_CONTEXT_OBJECT_HH
#define ETOILE_CONTEXT_OBJECT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/context/Context.hh>

#include <etoile/kernel/Object.hh>
#include <etoile/kernel/Access.hh>
#include <etoile/kernel/Author.hh>
#include <etoile/kernel/Genre.hh>

#include <etoile/hole/Address.hh>

#include <etoile/path/Route.hh>

#include <etoile/context/Rights.hh>
#include <etoile/context/Format.hh>

namespace etoile
{
  namespace context
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents an object context and is therefore used
    /// to perform sequential operations on objects.
    ///
    /// note that when the object is loaded or created, the rights the
    /// subject has over the object are computed.
    ///
    class Object:
      public Context
    {
    public:
      //
      // constructors & destructors
      //
      Object();
      Object(const Format&);
      ~Object();

      //
      // interfaces
      //

      // dumpable
      elle::Status	Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
      path::Route		route;
      hole::Address		address;

      kernel::Object*		object;
      kernel::Access*		access;

      Rights*			rights;
      kernel::Author*		author;
    };

  }
}

#endif
