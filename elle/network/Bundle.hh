//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/network/Bundle.hh
//
// created       julien quintard   [fri jun  3 22:23:13 2011]
// updated       julien quintard   [sun sep  4 11:56:11 2011]
//

#ifndef ELLE_NETWORK_BUNDLE_HH
#define ELLE_NETWORK_BUNDLE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/radix/Status.hh>
#include <elle/radix/Entity.hh>
#include <elle/radix/Arguments.hh>
#include <elle/radix/Variables.hh>
#include <elle/radix/Parameters.hh>

#include <elle/network/Tag.hh>
#include <elle/network/Message.hh>

namespace elle
{
  using namespace radix;

  namespace network
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a set of arguments associated with a network
    /// tag.
    ///
    /// note that this class is specialized for inputs (const) and ouptuts
    /// (non-const).
    ///
    struct Bundle
    {
      ///
      /// this generic Inputs class.
      ///
      template <const Tag G,
		typename... T>
      class Inputs;

      ///
      /// this class, specialized with Parameters<T...>, takes a set
      /// of values to bundle together so as to serialize them in order
      /// to send them over a network connection.
      ///
      template <const Tag G,
		typename... T>
      class Inputs< G, Parameters<T...> >:
	public Entity,
	public virtual Archivable
      {
      public:
	//
	// types
	//
	typedef Parameters<const T&...>				P;

	//
	// constructors & destructors
	//
	Inputs(const T&...);
	template <template <typename...> class E,
		  typename... U>
	Inputs(E< Parameters<U...> >&);

	//
	// interfaces
	//

	// archivable
	Status		Serialize(Archive&) const;
	Status		Extract(Archive&);

	// dumpable
	Status		Dump(const Natural32 = 0) const;

	//
	// attributes
	//
	Tag		tag;
	Arguments<P>	arguments;
      };

      ///
      /// the generic Outputs class.
      ///
      template <const Tag G,
		typename... T>
      class Outputs;

      ///
      /// like the Inputs class, this class is specialized for Parameters<T...>
      /// and is responsible for extracting values from a network packet so
      /// as to set the variables provided through the constructor.
      ///
      template <const Tag G,
		typename... T>
      class Outputs< G, Parameters<T...> >:
	public Entity,
	public virtual Archivable
      {
      public:
	//
	// types
	//
	typedef Parameters<T&...>				P;

	//
	// constructors & destructors
	//
	Outputs(T&...);
	template <template <typename...> class E,
		  typename... U>
	Outputs(E< Parameters<U...> >&);

	//
	// interfaces
	//

	// archivable
	Status		Serialize(Archive&) const;
	Status		Extract(Archive&);

	// dumpable
	Status		Dump(const Natural32 = 0) const;

	//
	// attributes
	//
	Tag		tag;
	Arguments<P>	arguments;
      };

    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/network/Bundle.hxx>

#endif
