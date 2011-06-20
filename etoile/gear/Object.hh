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
// updated       julien quintard   [mon jun 20 12:36:19 2011]
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
      Object(const Nature);
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
      nucleus::Object		object;
      nucleus::Access		access;

      // XXX once the object fetched, a base should be generated so that
      // it can be used later on.

      struct			Rights
      {
	nucleus::Role		role;
	nucleus::Permissions	permissions;
	elle::SecretKey		key;
      }*			rights;
      nucleus::Author*		author;
    };

  }
}

#endif
