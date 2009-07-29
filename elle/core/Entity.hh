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
// updated       julien quintard   [wed jul 29 14:00:32 2009]
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
      /// this destructor should release the allocated resources but must
      /// not re-set attributes to default values.
      ///
      virtual ~Entity()
      {

      }

      ///
      /// this constructor is called whenever:
      ///
      ///   Entity o1;
      ///   Entity o2 = o1;
      ///
      /// is used though the assignment operator is actually specified.
      ///
      /// therefore, this constructor stops the program. the developer should
      /// instead use the assignment operator:
      ///
      ///   Entity o1;
      ///   Entity o2;
      ///   o2 = o1;
      ///
      Entity(Entity&)
      {
	fail("copy constructors are not allowed to be used");
      }

      //
      // methods
      //

      ///
      /// this method recycles an entity by deallocating resources and
      /// re-setting them to their default values.
      ///
      template <typename T>
      Status		Recycle()
      {
	// release the resources.
	this->~Entity();

	// initialize the object with default values.
	new (this) T;
      }

      //
      // operators
      //

      ///
      /// this operator copies an entity.
      ///
      /// this method (i) starts by checking if the given entity
      /// is not the current one (ii) then calls the parent
      /// method---if relevant---before (iii) calling Recycle().
      /// finally, the method should (iv) assign the attributes including
      /// the entity-attributes by using the assignment operator.
      ///
      virtual Entity&	operator=(Entity&)
      {
	fail("this method should never have been called");
      }

      ///
      /// this operator compares two entitys.
      ///
      virtual Boolean	operator==(Entity&)
      {
	fail("this method should never have been called");
      }

      ///
      /// this operator compares two entitys.
      ///
      virtual Boolean	operator!=(Entity&)
      {
	fail("this method should never have been called");
      }
    };

  }
}

#endif
