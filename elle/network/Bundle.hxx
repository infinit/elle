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
// updated       julien quintard   [fri sep  2 22:12:55 2011]
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

    ///
    /// XXX
    ///
    template <const Tag G,
	      typename... T>
    Bundle::Inputs< G,
		    Parameters<T...> >::Inputs(const T&...	objects):
      tag(G),
      arguments(objects...)
    {
    }

    ///
    /// XXX
    ///
    template <const Tag G,
	      typename... T>
    template <template <typename...> class E,
	      typename... U>
    Bundle::Inputs< G,
		    Parameters<T...> >::Inputs(
					       E<
						 Parameters<
						   U...
						   >
						 >&		ensemble):
      tag(G),
      arguments(ensemble)
    {
    }

    ///
    /// XXX
    ///
    template <const Tag G,
	      typename... T>
    Status
    Bundle::Inputs< G,
		    Parameters<T...> >::Serialize(Archive&	archive) const
    {
      Callback<
	Status,
	typename Trait::Reference<
	  typename Trait::Constant<
	    typename Message<G>::P
	    >::Type
	  >::Type
	>		callback(&Archive::Serialize, &archive);

      enter();

      // trigger the serialization callback.
      if (this->arguments.Call(callback) == StatusError)
	escape("unable to serialize the arguments");

      leave();
    }

    ///
    /// XXX
    ///
    template <const Tag G,
	      typename... T>
    Status
    Bundle::Inputs< G,
		    Parameters<T...> >::Extract(Archive&)
    {
      enter();

      escape("unable to extract from an inputs bundle");
    }

    ///
    /// XXX
    ///
    template <const Tag G,
	      typename... T>
    Status
    Bundle::Inputs< G,
		    Parameters<T...> >::Dump(const Natural32	margin)
      const
    {
      String	alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Bundle] Inputs" << std::endl;

      // dump the tag.
      std::cout << alignment << Dumpable::Shift
		<< "[Tag] " << (Natural32)this->tag << std::endl;

      // dump the arguments.
      if (this->arguments.Dump() == StatusError)
	escape("unable to dump the arguments");

      leave();
    }

//
// ---------- outputs bundle --------------------------------------------------
//

    ///
    /// XXX
    ///
    template <const Tag G,
	      typename... T>
    Bundle::Outputs< G,
		     Parameters<T...> >::Outputs(T&...		objects):
      tag(G),
      arguments(objects...)
    {
    }

    ///
    /// XXX
    ///
    template <const Tag G,
	      typename... T>
    template <template <typename...> class E,
	      typename... U>
    Bundle::Outputs< G,
		     Parameters<T...> >::Outputs(
					   E<
					     Parameters<
					       U...
					       >
					     >&			ensemble):
      tag(G),
      arguments(ensemble)
    {
    }

    ///
    /// XXX
    ///
    template <const Tag G,
	      typename... T>
    Status
    Bundle::Outputs< G,
		     Parameters<T...> >::Serialize(Archive&) const
    {
      enter();

      escape("unable to serialize to an outputs bundle");
    }

    ///
    /// XXX
    ///
    template <const Tag G,
	      typename... T>
    Status
    Bundle::Outputs< G,
		     Parameters<T...> >::Extract(Archive& archive)
    {
      Callback<
	Status,
	typename Trait::Reference<
	  typename Message<G>::P
	  >::Type
	>		callback(&Archive::Extract, &archive);

      enter();

      // trigger the serialization callback.
      if (this->arguments.Call(callback) == StatusError)
	escape("unable to extract the arguments");

      leave();
    }


    ///
    /// XXX
    ///
    template <const Tag G,
	      typename... T>
    Status
    Bundle::Outputs< G,
		     Parameters<T...> >::Dump(const Natural32 margin)
      const
    {
      String	alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Bundle] Outputs" << std::endl;

      // dump the tag.
      std::cout << alignment << Dumpable::Shift
		<< "[Tag] " << (Natural32)this->tag << std::endl;

      // dump the arguments.
      if (this->arguments.Dump() == StatusError)
	escape("unable to dump the arguments");

      leave();
    }

  }
}

#endif
