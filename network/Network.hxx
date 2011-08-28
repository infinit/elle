//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/network/Network.hxx
//
// created       julien quintard   [wed feb  3 16:05:34 2010]
// updated       julien quintard   [sun aug 28 13:46:07 2011]
//

#ifndef ELLE_NETWORK_NETWORK_HXX
#define ELLE_NETWORK_NETWORK_HXX

//
// ---------- includes --------------------------------------------------------
//

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>

namespace elle
{
  namespace network
  {

//
// ---------- network ---------------------------------------------------------
//

    ///
    /// this method registers a callback associated with a set of types so that
    /// whenever a message is received, these types are extracted and the
    /// callback is triggered.
    ///
    template <const Tag I,
	      const Tag O>
    Status		Network::Register(const Procedure<I, O>& procedure)
    {
      Selectionoid< Procedure<I, O> >*		selectionoid;
      std::pair<Network::Iterator, Boolean>	result;

      enter(instance(selectionoid));

      // check if this tag has already been recorded.
      if (Network::Procedures.find(I) != Network::Procedures.end())
	escape("this tag seems to have already been recorded");

      // create a new selectionoid.
      selectionoid = new Selectionoid< Procedure<I, O> >(procedure);

      // insert the selectionoid in the container.
      result = Network::Procedures.insert(
	         std::pair<const Tag,
			   Network::Functionoid*>(I,
						  selectionoid));

      // check if the insertion was successful.
      if (result.second == false)
	escape("unable to insert the selectoinoid in the container");

      // waive the selectionoid tracking.
      waive(selectionoid);

      leave();
    }

//
// ---------- selectonoid -----------------------------------------------------
//

    ///
    /// default constructor.
    ///
    template <typename P>
    Network::Selectionoid<P>::Selectionoid(const P&		procedure):
      procedure(procedure)
    {
    }

    ///
    /// this method forwards the request by calling the procedure's
    /// Skeleton() method.
    ///
    template <typename P>
    Status	Network::Selectionoid<P>::Call(Archive&		archive) const
    {
      enter();

      // call the procedure's skeleton.
      if (this->procedure.Skeleton(archive) == StatusError)
	escape("an error occured in the procedure's skeleton");

      leave();
    }

    ///
    /// this method dumps the selectionoid.
    ///
    template <typename P>
    Status	Network::Selectionoid<P>::Dump(const Natural32	margin) const
    {
      enter();

      // dump the object.
      if (this->procedure.Dump(margin) == StatusError)
	escape("unable to dump the object");

      leave();
    }

  }
}

#endif
