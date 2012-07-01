#ifndef ELLE_RADIX_ENTITY_HH
# define ELLE_RADIX_ENTITY_HH

#include <elle/radix/Meta.hh>

namespace elle
{
  namespace radix
  {

    ///
    /// this interface must be implemented by every 'entity' ... in the
    /// Elle library terms.
    ///
    /// an entity is an instance that cannot be assigned, serialized
    /// hence stored on external media.
    ///
    class Entity:
      public Meta
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
