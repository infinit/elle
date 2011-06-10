//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/network/Bundle.hxx
//
// created       julien quintard   [fri jun  3 22:22:21 2011]
// updated       julien quintard   [fri jun 10 12:42:40 2011]
//

#ifndef ELLE_NETWORK_BUNDLE_HXX
#define ELLE_NETWORK_BUNDLE_HXX

//
// ---------- includes --------------------------------------------------------
//

#include <elle/radix/Status.hh>
#include <elle/radix/Entity.hh>
#include <elle/radix/Trait.hh>

#include <elle/package/Archivable.hh>

namespace elle
{
  using namespace radix;
  using namespace package;

  namespace network
  {

//
// ---------- inputs bundle ---------------------------------------------------
//

    template <const Tag G,
	      typename... T>
    class Bundle< G, Parameters<const T...> >:
      public Entity,
      public virtual Archivable
    {
    public:
      //
      // types
      //
      typedef Parameters<const T&...>	P;

      //
      // constructors & destructors
      //
      Bundle(const T&...					objects):
	tag(G),
	arguments(objects...)
      {
      }

      //
      // interfaces
      //

      // archivable

      Status		Serialize(Archive&			archive)
	const
      {
	Callback<
	  typename Trait::Reference<
 	    typename Trait::Constant<
	      typename Message<G>::P
	      >::Type
	    >::Type
	  >		callback(&Archive::Serialize, &archive);

	enter();

	// trigger the serialization callback.
	if (this->arguments.Trigger(callback) == StatusError)
	  escape("unable to serialize the arguments");

	leave();
      }

      Status		Extract(Archive&)
      {
	enter();

	escape("unable to extract from an inputs bundle");
      }

      // dumpable

      Status		Dump(const Natural32			margin = 0)
	const
      {
	String		alignment(margin, ' ');

	enter();

	std::cout << alignment << "[Bundle] Inputs" << std::endl;

	// dump the tag.
	std::cout << alignment << Dumpable::Shift
		  << "[Tag]" << (Natural32)this->tag << std::endl;

	// dump the arguments.
	if (this->arguments.Dump() == StatusError)
	  escape("unable to dump the arguments");

	leave();
      }

      //
      // attributes
      //
      Tag		tag;
      Arguments<P>	arguments;
    };

//
// ---------- outputs bundle --------------------------------------------------
//

    template <const Tag G,
	      typename... T>
    class Bundle< G, Parameters<T...> >:
      public Entity,
      public virtual Archivable
    {
    public:
      //
      // types
      //
      typedef Parameters<T&...>		P;

      //
      // constructors & destructors
      //
      Bundle(T&...						objects):
	tag(G),
	arguments(objects...)
      {
      }

      //
      // interfaces
      //

      // archivable

      Status		Serialize(Archive&			archive) const
      {
	enter();

	escape("unable to serialize to an output bundle");
      }

      Status		Extract(Archive&			archive)
      {
	Callback<
	  typename Trait::Reference<
	    typename Message<G>::P
	    >::Type
	  >		callback(&Archive::Extract, &archive);

	enter();

	// trigger the serialization callback.
	if (this->arguments.Trigger(callback) == StatusError)
	  escape("unable to extract the arguments");

	leave();
      }

      // dumpable

      Status		Dump(const Natural32			margin = 0)
	const
      {
	String		alignment(margin, ' ');

	enter();

	std::cout << alignment << "[Bundle] Outputs" << std::endl;

	// dump the tag.
	std::cout << alignment << Dumpable::Shift
		  << "[Tag]" << (Natural32)this->tag << std::endl;

	// dump the arguments.
	if (this->arguments.Dump() == StatusError)
	  escape("unable to dump the arguments");

	leave();
      }

      //
      // attributes
      //
      Tag		tag;
      Arguments<P>	arguments;
    };

  }
}

#endif
