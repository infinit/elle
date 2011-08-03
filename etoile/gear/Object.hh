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
// updated       julien quintard   [fri jul 29 14:31:57 2011]
//

#ifndef ETOILE_GEAR_OBJECT_HH
#define ETOILE_GEAR_OBJECT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>

#include <etoile/gear/Context.hh>
#include <etoile/gear/Nature.hh>
 
namespace etoile
{
  namespace gear
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents an object context.
    ///
    /// although every object references a Contents block which itself
    /// contains the address of the data blocks, the Contents block is
    /// not included in this context as it depends on the object's genre i.e
    /// file, directory or link.
    ///
    /// however, object contexts include an access block which may be missing
    /// should the object be kept private to the owner.
    ///
    /// finally, note that once necessary, the user's rights on the
    /// object are computed; likewise for the authorship.
    ///
    /// noteworthy is that the base is computed when the object is
    /// fetched from the storage layer, hence sealing its original state.
    ///
    /// a context can only be stored, destroyed or discarded.
    ///
    /// in the first case, the set of interconnected blocks are made
    /// consistent and the internal addresses are re-computed to reference
    /// the other blocks.
    ///
    /// ih the second case, since the blocks have not been made consistent, no
    /// matter what modifications have been performed, the original address
    /// plus the internal addresses remain unchanged and can be used to
    /// remove the blocks from the storage layer.
    ///
    /// in the last case, the context is simply dropped, hence ignoring
    /// the potential modifications.
    ///
    class Object:
      public Context
    {
    public:
      //
      // constructors & destructors
      //
      Object();
      Object(const Nature);
      ~Object();

      //
      // interfaces
      //

      // dumpable
      elle::Status	Dump(const elle::Natural32 = 0) const;

      // archivable
      elle::Status	Serialize(elle::Archive&) const;
      elle::Status	Extract(elle::Archive&);

      //
      // attributes
      //
      nucleus::Location		location;

      nucleus::Object		object;
      nucleus::Access*		access;

      struct
      {
	nucleus::Role		role;
	nucleus::Permissions	permissions;
	elle::SecretKey		key;
	nucleus::Record		record;
      }				rights;
      nucleus::Author		author;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <etoile/gear/Object.hxx>

#endif
