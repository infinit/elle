//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/core/Entity.hh
//
// created       julien quintard   [sun feb 22 19:43:33 2009]
// updated       julien quintard   [mon aug  3 20:56:15 2009]
//

#ifndef ELLE_CORE_ENTITY_HH
#define ELLE_CORE_ENTITY_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Boolean.hh>

#include <elle/misc/Status.hh>
#include <elle/misc/Report.hh>

namespace elle
{
  using namespace misc;

  namespace core
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this interface must be implemented by every 'entity' ... in the
    /// elle library terms.
    ///
    class Entity
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
      /// the copy constructor is responsible for duplicating an entity
      /// and should therefore be provided for every class also providing
      /// the = operator.
      ///
      Entity(const Entity&)
      {
      }

      ///
      /// this destructor should release the allocated resources but must
      /// not re-set attributes to default values.
      ///
      virtual ~Entity()
      {
      }

      //
      // methods
      //

      ///
      /// this method recycles an entity by deallocating resources and
      /// re-setting them to their default values.
      ///
      template <typename T>
      Status		Recycle(const T*			entity = NULL)
      {
	// release the resources.
	this->~Entity();

	if (entity == NULL)
	  {
	    // initialize the object with default values.
	    new (this) T;
	  }
	else
	  {
	    // initialize the object with defined values.
	    new (this) T(*entity);
	  }

	leave();
      }

      //
      // operators
      //

      ///
      /// this operator copies an entity.
      ///
      /// this method (i) starts by checking if the given entity
      /// is not the current one before (ii) calling Recycle().
      ///
      virtual Entity&	operator=(const Entity&)
      {
	fail("this method should never have been called");
      }

      ///
      /// this operator compares two entitys.
      ///
      virtual Boolean	operator==(const Entity&) const
      {
	fail("this method should never have been called");
      }

      ///
      /// this operator compares two entitys.
      ///
      virtual Boolean	operator!=(const Entity&) const
      {
	fail("this method should never have been called");
      }
    };

  }
}

#endif
