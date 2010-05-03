//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/radix/Entity.hh
//
// created       julien quintard   [sun feb 22 19:43:33 2009]
// updated       julien quintard   [sun may  2 12:19:05 2010]
//

#ifndef ELLE_RADIX_ENTITY_HH
#define ELLE_RADIX_ENTITY_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/radix/Meta.hh>

#include <elle/io/Dumpable.hh>

namespace elle
{
  using namespace radix;
  using namespace io;

  namespace radix
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this interface must be implemented by every 'entity' ... in the
    /// elle library terms.
    ///
    /// an entity is an instance that cannot be assigned, serialized
    /// hence stored on external media.
    ///
    /// however, an entity, like for objects, can be dumped.
    ///
    class Entity:
      public Meta,
      public virtual Dumpable
    {
    public:
      //
      // constructors and destructors
      //

      ///
      /// every class inheriting directly or indirectly the Entity interface
      /// must define a default constructor.
      ///
      /// note that nothing---especially no allocation---should be performed
      /// in this method but initializing the attributes to default values.
      ///
      Entity()
      {
      }

      ///
      /// this destructor should release the allocated resources but must
      /// not re-set attributes to default values.
      ///
      virtual ~Entity()
      {
      }
    };

  }
}

#endif
