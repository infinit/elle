//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/package/Derivable.hh
//
// created       julien quintard   [fri may 13 12:47:04 2011]
// updated       julien quintard   [fri jul  8 10:42:16 2011]
//

#ifndef ELLE_PACKAGE_DERIVABLE_HH
#define ELLE_PACKAGE_DERIVABLE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Natural.hh>

#include <elle/radix/Status.hh>
#include <elle/radix/Object.hh>

#include <elle/utility/Factory.hh>

namespace elle
{
  using namespace core;
  using namespace radix;
  using namespace utility;

  namespace package
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class enables the programmer to retrieve derivable classes
    /// from archives.
    ///
    /// note that the actual class must be registered at the factory
    /// but also provided whenever serialized/extracted.
    ///
    /// then, when receiving a Derivable<T> object, the programmer can
    /// call the Infer() method which provides a pointer upon the derived
    /// instance.
    ///
    /// note that, should the object be allocated by the factory, the
    /// programmer does not have to worry about releasing the object
    /// since this is taken care of by the Derivable class.
    ///
    template <typename T>
    class Derivable:
      public Object
    {
    public:
      //
      // enumerations
      //
      enum Policy
	{
	  PolicyUnknown,
	  PolicyDynamic,
	  PolicyStatic
	};

      //
      // constructors & destructors
      //
      Derivable();
      Derivable(const Product&,
		const T&);
      Derivable(const Factory&);
      Derivable(const Factory&,
		const T&);

      ~Derivable();

      //
      // methods
      //
      template <typename U>
      Status		Infer(U*&) const;

      //
      // interfaces
      //

      // object
      declare(Derivable);

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      // archivable
      Status		Serialize(Archive&) const;
      Status		Extract(Archive&);

      //
      // attributes
      //
      Policy		policy;
      const Factory*	factory;
      Product		product;
      T*		object;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/package/Derivable.hxx>

#endif
