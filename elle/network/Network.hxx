//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [wed feb  3 16:05:34 2010]
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
              const Tag O,
              const Tag E>
    Status              Network::Register(const Procedure<I, O, E>& procedure)
    {
      std::pair<Network::Iterator, Boolean>     result;

      // check if this tag has already been recorded.
      if (Network::Procedures.find(I) != Network::Procedures.end())
        escape("this tag seems to have already been recorded");

      // create a new selectionoid.
      auto                                      selectionoid =
        std::unique_ptr< Selectionoid< Procedure<I, O, E> > >(
          new Selectionoid< Procedure<I, O, E> >(procedure));

      // insert the selectionoid in the container.
      result =
        Network::Procedures.insert(
          std::pair<const Tag, Network::Functionoid*>(I, selectionoid.get()));

      // check if the insertion was successful.
      if (result.second == false)
        escape("unable to insert the selectoinoid in the container");

      // release tracking.
      selectionoid.release();

      return Status::Ok;
    }

//
// ---------- selectonoid -----------------------------------------------------
//

    ///
    /// default constructor.
    ///
    template <typename P>
    Network::Selectionoid<P>::Selectionoid(const P&             procedure):
      procedure(procedure)
    {
    }

    ///
    /// this method forwards the request by calling the procedure's
    /// Skeleton() method.
    ///
    template <typename P>
    Status      Network::Selectionoid<P>::Call(Archive&         archive) const
    {
      return (this->procedure.Skeleton(archive));
    }

    ///
    /// this method dumps the selectionoid.
    ///
    template <typename P>
    Status      Network::Selectionoid<P>::Dump(const Natural32  margin) const
    {
      // dump the object.
      if (this->procedure.Dump(margin) == Status::Error)
        escape("unable to dump the object");

      return Status::Ok;
    }

  }
}

#endif
