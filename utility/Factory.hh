//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/utility/Factory.hh
//
// created       julien quintard   [thu jan 28 18:47:33 2010]
// updated       julien quintard   [mon jul 11 15:39:03 2011]
//

#ifndef ELLE_UTILITY_FACTORY_HH
#define ELLE_UTILITY_FACTORY_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Natural.hh>

#include <elle/radix/Status.hh>
#include <elle/radix/Object.hh>
#include <elle/radix/Entity.hh>

#include <elle/utility/Product.hh>

#include <elle/idiom/Close.hh>
# include <map>
#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace core;
  using namespace radix;

  namespace utility
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class holds the mappings between identifiers and the functionoids
    /// capable of generating types.
    ///
    class Factory:
      public Object
    {
    public:
      //
      // classes
      //

      ///
      /// this class is the base class for factory functionoids.
      ///
      class Functionoid:
	public Entity
      {
      public:
	//
	// constructors & destructors
	//
	virtual ~Functionoid()
	{
	}

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
	virtual Status	Allocate(Meta*&) const = 0;
      };

      ///
      /// this functionoid contains a method for generating a Meta object
      /// of the given type.
      ///
      template <typename T>
      class Generatoid:
	public Functionoid
      {
      public:
	//
	// constructors & destructors
	//
	Generatoid(const Product&);

	//
	// methods
	//
	Status		Allocate(Meta*&) const;

	//
	// attributes
	//
	Product		identifier;
      };

      //
      // types
      //
      typedef std::map<const Product, Functionoid*>	Container;
      typedef Container::iterator			Iterator;
      typedef Container::const_iterator			Scoutor;

      //
      // constructors & destructors
      //
      ~Factory();

      //
      // methods
      //
      template <typename T>
      Status		Register(const Product&);
      template <typename U>
      Status		Build(const Product&,
			      U*&) const;

      //
      // interfaces
      //

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      Container		container;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/utility/Factory.hxx>

#endif
