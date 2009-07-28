//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /data/mycure/repositories/infinit/elle/core/Entity.hh
//
// created       julien quintard   [sun feb 22 19:43:33 2009]
// updated       julien quintard   [tue jul 28 18:35:06 2009]
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
      /// must define a default constructor and destructor that will call
      /// the New() and Delete() methods, respectively.
      ///
      Entity()
      {
      }

      ///
      /// this constructors is called when:
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
      /// this method initializes the internal attributes.
      ///
      /// note that nothing---especially no allocation---should be
      /// performed in this method but initializing the attributes
      /// to default values.
      ///
      /// however, note that this method must *not* call the
      /// parent class' New() method nor should it call
      /// New() on its entity-attributes.
      ///
      /// note that this method is not virtual since static methods
      /// cannot be declared as virtual. however, every class deriving
      /// Entity should provide them.
      ///
      /// for more information, these methods are declared as static
      /// to avoid polymorphism calls.
      ///
      static Status	New(Entity&)
      {
	fail("this method should never have been called");
      }

      ///
      /// this method releases the allocated resources.
      ///
      /// finally, this method must *not* call the parent Delete()
      /// method, nor should it call the New() method, nor should
      /// it call Delete() on its entity-attributes.
      ///
      static Status	Delete(Entity&)
      {
	fail("this method should never have been called");
      }

      //
      // operators
      //

      ///
      /// this operator copies an entity.
      ///
      /// this method (i) starts by checking if the given entity
      /// is not the current one (ii) then calls the parent
      /// method---if relevant---before (iii) calling Delete() to
      /// release the resources (iv) and New() to reinitialize
      /// the attributes. finally, the method should (v) assign
      /// the attributes including the entity-attributes by
      /// using the assignment operator.
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
