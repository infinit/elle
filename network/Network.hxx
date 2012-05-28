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
      Function f(boost::bind(&Procedure<I, O, E>::Skeleton,
                             Procedure<I, O, E>(procedure), _1, _2, _3));
      return Register(I, f);
    }
  }
}

#endif
