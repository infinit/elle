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
// updated       julien quintard   [mon may  3 21:14:50 2010]
//

#ifndef ELLE_FACTORY_FACTORY_HH
#define ELLE_FACTORY_FACTORY_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Natural.hh>
#include <elle/core/String.hh>

#include <elle/radix/Status.hh>
#include <elle/radix/Entity.hh>

#include <elle/concurrency/Accord.hh>

#include <elle/idiom/Close.hh>
# include <map>
#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace core;
  using namespace radix;
  using namespace concurrency;

  ///
  /// this namespace contains components related to the generations of types.
  ///
  namespace factory
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class holds the mappings between identifiers and the functionoids
    /// capable of generating types.
    ///
    class Factory
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
      class Generator:
	public Functionoid
      {
      public:
	//
	// constructors & destructors
	//
	Generator(const String&);

	//
	// methods
	//
	Status		Allocate(Meta*&) const;

	//
	// attributes
	//
	String		identifier;
      };

      //
      // types
      //
      typedef std::map<const String,
		       Functionoid*>		Container;
      typedef Container::iterator		Iterator;
      typedef Container::const_iterator		Scoutor;

      //
      // static methods
      //
      static Status	Initialize();
      static Status	Clean();

      template <typename T>
      static Status	Register(const String&);

      template <typename U>
      static Status	Build(const String&,
			      U*&);

      static Status	Show(const Natural32 = 0);

      //
      // attributes
      //
      static Accord	Control;

      static Container	Map;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/factory/Factory.hxx>

#endif
