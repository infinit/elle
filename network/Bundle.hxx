//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [fri jun  3 22:22:21 2011]
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
    /// default constructor.
    ///
    template <const Tag G,
              typename... T>
    Bundle::Inputs< G,
                    Parameters<T...> >::Inputs(const T&...      objects):
      tag(G),
      arguments(objects...)
    {
    }

    ///
    /// a specific constructor for ensembles i.e Arguments or Variables
    /// for instance.
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
                                                 >&             ensemble):
      tag(G),
      arguments(ensemble)
    {
    }

    ///
    /// this method serializes the variables referenced through the Arguments
    /// attribute.
    ///
    template <const Tag G,
              typename... T>
    Status
    Bundle::Inputs< G,
                    Parameters<T...> >::Serialize(Archive&      archive) const
    {
      Callback<
        Status::,
        typename Trait::Reference<
          typename Trait::Constant<
            typename Message<G>::P
            >::Type
          >::Type
        >               callback(&Archive::Serialize, &archive);

      // trigger the serialization callback.
      if (this->arguments.Call(callback) == Status::Error)
        escape("unable to serialize the arguments");

      return Status::Ok;
    }

    ///
    /// this method does nothing when it comes to Inputs bundles.
    ///
    template <const Tag G,
              typename... T>
    Status
    Bundle::Inputs< G,
                    Parameters<T...> >::Extract(Archive&)
    {
      escape("unable to extract from an inputs bundle");
    }

    ///
    /// this method dumps the bundle.
    ///
    template <const Tag G,
              typename... T>
    Status
    Bundle::Inputs< G,
                    Parameters<T...> >::Dump(const Natural32    margin)
      const
    {
      String    alignment(margin, ' ');

      std::cout << alignment << "[Bundle] Inputs" << std::endl;

      // dump the tag.
      std::cout << alignment << Dumpable::Shift
                << "[Tag] " << static_cast<Natural32>(this->tag) << std::endl;

      // dump the arguments.
      if (this->arguments.Dump() == Status::Error)
        escape("unable to dump the arguments");

      return Status::Ok;
    }

//
// ---------- outputs bundle --------------------------------------------------
//

    ///
    /// default constructor.
    ///
    template <const Tag G,
              typename... T>
    Bundle::Outputs< G,
                     Parameters<T...> >::Outputs(T&...          objects):
      tag(G),
      arguments(objects...)
    {
    }

    ///
    /// a specific constructor for ensembles i.e Arguments or Variables
    /// for instance.
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
                                             >&                 ensemble):
      tag(G),
      arguments(ensemble)
    {
    }

    ///
    /// this method does nothing when it comes to Outputs bundles.
    ///
    template <const Tag G,
              typename... T>
    Status
    Bundle::Outputs< G,
                     Parameters<T...> >::Serialize(Archive&) const
    {
      escape("unable to serialize to an outputs bundle");
    }

    ///
    /// this method extract information from the given archive in order
    /// to set the values of the variables referenced through the Arguments
    /// attribute.
    ///
    template <const Tag G,
              typename... T>
    Status
    Bundle::Outputs< G,
                     Parameters<T...> >::Extract(Archive&       archive)
    {
      Callback<
        Status::,
        typename Trait::Reference<
          typename Message<G>::P
          >::Type
        >               callback(&Archive::Extract, &archive);

      // trigger the serialization callback.
      if (this->arguments.Call(callback) == Status::Error)
        escape("unable to extract the arguments");

      return Status::Ok;
    }


    ///
    /// this method dumps the bundle.
    ///
    template <const Tag G,
              typename... T>
    Status
    Bundle::Outputs< G,
                     Parameters<T...> >::Dump(const Natural32 margin)
      const
    {
      String    alignment(margin, ' ');

      std::cout << alignment << "[Bundle] Outputs" << std::endl;

      // dump the tag.
      std::cout << alignment << Dumpable::Shift
                << "[Tag] " << static_cast<Natural32>(this->tag) << std::endl;

      // dump the arguments.
      if (this->arguments.Dump() == Status::Error)
        escape("unable to dump the arguments");

      return Status::Ok;
    }

  }
}

#endif
