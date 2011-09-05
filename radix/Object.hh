//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/radix/Object.hh
//
// created       julien quintard   [sun feb 22 19:43:33 2009]
// updated       julien quintard   [sat sep  3 20:51:58 2011]
//

#ifndef ELLE_RADIX_OBJECT_HH
#define ELLE_RADIX_OBJECT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Boolean.hh>
#include <elle/core/Natural.hh>

#include <elle/radix/Meta.hh>
#include <elle/radix/Status.hh>

#include <elle/package/Archivable.hh>

#include <elle/io/Dumpable.hh>
#include <elle/io/Format.hh>
#include <elle/io/Uniquable.hh>

#include <elle/concurrency/Resource.hh>

namespace elle
{
  using namespace core;
  using namespace package;
  using namespace io;
  using namespace concurrency;

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
    /// note that an object implicitly acts as a concurrency control resource.
    ///
    class Object:
      public Meta,
      public Resource,
      public virtual Dumpable,
      public virtual Archivable,
      public virtual Uniquable<FormatBase64>
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
      Status		Recycle(const T* = NULL);

      ///
      /// this method returns the size of the object.
      ///
      virtual Status	Imprint(Natural32&) const;

      ///
      /// this method clones the current object.
      ///
      virtual Status	Clone(Object*&) const;

      //
      // operators
      //
      virtual Boolean	operator==(const Object&) const;
      virtual Boolean	operator<(const Object&) const;
      virtual Boolean	operator>(const Object&) const;
      virtual Object	operator+(const Object&);
      virtual Object&	operator=(const Object&);
      virtual Boolean	operator!=(const Object&) const;
      virtual Boolean	operator<=(const Object&) const;
      virtual Boolean	operator>=(const Object&) const;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/radix/Object.hxx>

#endif
