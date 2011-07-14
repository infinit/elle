//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/utility/Registrar.hh
//
// created       julien quintard   [mon jun  6 12:07:32 2011]
// updated       julien quintard   [mon jul 11 14:47:43 2011]
//

#ifndef ELLE_UTILITY_REGISTRAR_HH
#define ELLE_UTILITY_REGISTRAR_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Natural.hh>

#include <elle/radix/Status.hh>
#include <elle/radix/Object.hh>
#include <elle/radix/Variables.hh>

#include <elle/package/Archive.hh>

#include <elle/idiom/Close.hh>
# include <map>
#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace core;
  using namespace radix;
  using namespace package;

  namespace utility
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class associates identifier with a callback, closure or else
    /// so that whenever an archive is received, it can be transformed
    /// into its associated function call by simply calling the
    /// Forward() method.
    ///
    /// this class can be used, for instance, for associating network
    /// messages, which are identified by a tag, to a method for handling them.
    ///
    /// BEWARE! this mechanism is not type safe as it relies either on
    /// serialization or casting in order to transform dynamic information
    /// into static i.e type-specific calls.
    ///
    template <typename T>
    class Registrar:
      public Object
    {
    public:
      //
      // classes
      //

      ///
      /// this class represents the functionoid used to forward the call.
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
	virtual Status	Call(Archive&) const = 0;
      };

      ///
      /// this implementation takes an archive, extracts a number of
      /// variables depending on the selectionoid and triggers the
      /// callback, closure or else.
      ///
      template <typename C>
      class Selectionoid:
	public Functionoid
      {
      public:
	//
	// constructors & destructors
	//
	Selectionoid(const C&);

	//
	// methods
	//
	Status		Call(Archive&) const;

	//
	// interfaces
	//

	// dumpable
	Status		Dump(const Natural32) const;

	//
	// attributes
	//
	C		object;
      };

      //
      // types
      //
      typedef std::map<const T, Functionoid*>		Container;
      typedef typename Container::iterator		Iterator;
      typedef typename Container::const_iterator	Scoutor;

      //
      // constructors & destructors
      //
      ~Registrar();

      //
      // methods
      //
      template <typename C>
      Status		Register(const T,
				 const C&);
      Status		Dispatch(const T,
				 Archive&);

      //
      // interfaces
      //

      // object
      declare(Registrar<T>);

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      //
      // static attributes
      //
      Container		container;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/utility/Registrar.hxx>

#endif
