//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       GPL
//
// file          /home/mycure/infinit/elle/core/Object.hh
//
// created       julien quintard   [sun feb 22 19:43:33 2009]
// updated       julien quintard   [wed mar 11 15:55:41 2009]
//

#ifndef ELLE_CORE_OBJECT_HH
#define ELLE_CORE_OBJECT_HH

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
    /// this interface must be implemented by every 'object' ... in the
    /// elle library terms.
    ///
    class Object
    {
    public:
      //
      // constructors and destructors
      //

      ///
      /// every class inheriting directly or indirectly the Object interface
      /// must define a default constructor and destructor that will call
      /// the New() and Delete() methods, respectively.
      ///
      Object()
      {
      }

      ///
      /// this constructors is called when:
      ///
      ///   Object o1;
      ///   Object o2 = o1;
      ///
      /// is used though the assignment operator is actually specified.
      ///
      /// therefore, this constructor stops the program. the developer should
      /// instead use the assignment operator:
      ///
      ///   Object o1;
      ///   Object o2;
      ///   o2 = o1;
      ///
      Object(Object&)
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
      /// New() on its object-attributes.
      ///
      /// note that this method is not virtual since static methods
      /// cannot be declared as virtual. however, every class deriving
      /// Object should provide them.
      ///
      /// for more information, these methods are declared as static
      /// to avoid polymorphism calls.
      ///
      static Status	New(Object&)
      {
	fail("this method should never have been called");
      }

      ///
      /// this method releases the allocated resources.
      ///
      /// finally, this method must *not* call the parent Delete()
      /// method, nor should it call the New() method, nor should
      /// it call Delete() on its object-attributes.
      ///
      static Status	Delete(Object&)
      {
	fail("this method should never have been called");
      }

      //
      // operators
      //

      ///
      /// this operator copies an object.
      ///
      /// this method (i) starts by checking if the given object
      /// is not the current one (ii) then calls the parent
      /// method---if relevant---before (iii) calling Delete() to
      /// release the resources (iv) and New() to reinitialize
      /// the attributes. finally, the method should (v) assign
      /// the attributes including the object-attributes by
      /// using the assignment operator.
      ///
      virtual Object&	operator=(Object&)
      {
	fail("this method should never have been called");
      }

      ///
      /// this operator compares two objects.
      ///
      virtual Boolean	operator==(Object&)
      {
	fail("this method should never have been called");
      }

      ///
      /// this operator compares two objects.
      ///
      virtual Boolean	operator!=(Object&)
      {
	fail("this method should never have been called");
      }
    };

  }
}

#endif
