//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/gear/Object.hh
//
// created       julien quintard   [fri aug 14 23:13:51 2009]
// updated       julien quintard   [fri jun  3 11:55:08 2011]
//

#ifndef ETOILE_GEAR_OBJECT_HH
#define ETOILE_GEAR_OBJECT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>

#include <etoile/gear/Context.hh>

namespace etoile
{
  namespace gear
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

      // XXX object

      // dumpable
      elle::Status	Dump(const elle::Natural32 = 0) const;

      // XXX archivable

      //
      // attributes
      //
      nucleus::Object*		object;
      nucleus::Access*		access;
    };

  }
}

#endif
