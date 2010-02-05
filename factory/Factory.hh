//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/factory/Factory.hh
//
// created       julien quintard   [thu jan 28 18:47:33 2010]
// updated       julien quintard   [fri jan 29 10:46:12 2010]
//

#ifndef ELLE_FACTORY_FACTORY_HH
#define ELLE_FACTORY_FACTORY_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Core.hh>

#include <map>

namespace elle
{
  using namespace core;

  ///
  /// this namespace contains components related to the generations of types.
  ///
  namespace factory
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class is the base class for factory functionoids.
    ///
    class FactoryFunctionoid
    {
    public:
      //
      // methods
      //

      ///
      /// this method must be redefined in every sub-class.
      ///
      /// note that using a template method would have been better but
      /// since template methods cannot be virtual, the argument is assumed
      /// to be a derived entity.
      ///
      virtual Status	Allocate(Entity*&) const = 0;
    };

    ///
    /// this class holds the mappings between identifiers and the functionoids
    /// capable of generating types.
    ///
    class Factory
    {
    public:
      //
      // types
      //
      typedef std::map<const String,
		       FactoryFunctionoid*>	Container;
      typedef Container::iterator		Iterator;
      typedef Container::const_iterator		Scoutor;

      //
      // static methods
      //
      template <typename T>
      static Status	Register(const String&);

      template <typename U>
      static Status	Build(const String&,
			      U*&);

      static Status	Dump(const Natural32 = 0);

      //
      // attributes
      //
      static Container	Map;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/factory/Factory.hxx>

#endif
