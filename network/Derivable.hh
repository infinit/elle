//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/network/Derivable.hh
//
// created       julien quintard   [fri may 13 12:47:04 2011]
// updated       julien quintard   [fri may 13 15:50:45 2011]
//

#ifndef ELLE_NETWORK_DERIVABLE_HH
#define ELLE_NETWORK_DERIVABLE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Natural.hh>

#include <elle/radix/Status.hh>
#include <elle/radix/Object.hh>

#include <elle/factory/Factory.hh>

namespace elle
{
  using namespace core;
  using namespace radix;
  using namespace factory;

  namespace network
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class enables the programmer to receive derivable classes
    /// in messages.
    ///
    /// note that the actual class must be registered at the factory
    /// but also provided whenever sent.
    ///
    /// then, when receiving a Derivable<T> object, the programmer can
    /// call the Infer() method which provides a pointer upon the derived
    /// instance.
    ///
    /// note that the programmer does not have to worry about releasing the
    /// object since this is taken care of by the Derivable class.
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
	  PolicyDynamic,
	  PolicyStatic
	};

      //
      // constructors & destructors
      //
      Derivable();
      Derivable(const Factory::Identifier&,
		const T&);
      Derivable(const T&);
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
      Policy			policy;
      Factory::Identifier	identifier;
      T*			object;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/network/Derivable.hxx>

#endif
