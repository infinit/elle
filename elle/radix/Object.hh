#ifndef ELLE_RADIX_OBJECT_HH
#define ELLE_RADIX_OBJECT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/types.hh>

#include <elle/radix/Meta.hh>

namespace elle
{

  namespace radix
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this interface must be implemented by every 'object' ... in the
    /// Elle library terms.
    ///
    /// an object is an instance of a class which can be copied through
    /// assignment, which can be recycled but also compared. in addition,
    /// an object can be serialized hence stored offline such as in a file.
    ///
    class Object:
      public Meta
    {
    public:
      //
      // constructors and destructors
      //

      ///
      /// every class inheriting directly or indirectly the Object interface
      /// must define a default constructor.
      ///
      /// note that nothing---especially no allocation---should be performed
      /// in this method but initializing the attributes to default values.
      ///
      Object()
      {
      }

      ///
      /// the copy constructor is responsible for duplicating an object
      /// and should therefore be provided for every class also providing
      /// the = operator.
      ///
      Object(const Object&)
      {
      }

      ///
      /// this destructor should release the allocated resources but must
      /// not re-set attributes to default values.
      ///
      virtual ~Object()
      {
      }

      //
      // methods
      //

      ///
      /// this method recycles an object by deallocating resources and
      /// re-setting them to their default values.
      ///
      template <typename T>
      Status            Recycle(const T* = NULL);

      ///
      /// this method returns the size of the object.
      ///
      virtual Status    Imprint(Natural32&) const;

      ///
      /// this method clones the current object.
      ///
      virtual Status    Clone(Object*&) const;

      //
      // operators
      //
      virtual Boolean   operator==(const Object&) const;
      virtual Boolean   operator<(const Object&) const;
      virtual Boolean   operator>(const Object&) const;
      virtual Object    operator+(const Object&);
      virtual Object&   operator=(const Object&);
      virtual Boolean   operator!=(const Object&) const;
      virtual Boolean   operator<=(const Object&) const;
      virtual Boolean   operator>=(const Object&) const;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/radix/Object.hxx>

#endif
