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
// updated       julien quintard   [tue apr  6 23:09:40 2010]
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
      Status		Dump(const Natural32 = 0) const;

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
