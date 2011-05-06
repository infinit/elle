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
// updated       julien quintard   [thu may  5 16:26:35 2011]
//

#ifndef ETOILE_CONTEXT_OBJECT_HH
#define ETOILE_CONTEXT_OBJECT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>

#include <etoile/context/Context.hh>

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
      nucleus::Address		address;

      nucleus::Object*		object;
      nucleus::Access*		access;

      Rights*			rights;
      nucleus::Author*		author;
    };

  }
}

#endif
