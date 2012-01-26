//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       elle
//
// author        julien quintard   [mon apr 25 11:11:14 2011]
//

#ifndef ELLE_IO_PATH_HXX
#define ELLE_IO_PATH_HXX

//
// ---------- includes --------------------------------------------------------
//

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>

namespace elle
{
  using namespace core;

  namespace io
  {

//
// ---------- variadic templates ----------------------------------------------
//

    ///
    /// this template resolves a single piece of the path's pattern.
    ///
    template <typename T>
    Status              Path::Complete(T                        piece)
    {
      enter();

      // resolve the piece.
      if (this->Complete(piece.name, piece.value) == StatusError)
        escape("unable to resolve the piece");

      leave();
    }

    ///
    /// this method resolves the given set of pieces for the path's pattern.
    ///
    template <typename T,
              typename... TT>
    Status              Path::Complete(T                        piece,
                                       TT...                    pieces)
    {
      enter();

      // resolve the given piece.
      if (this->Complete(piece) == StatusError)
        escape("unable to resolve the first piece");

      // resolve the additional pieces.
      if (this->Complete(pieces...) == StatusError)
        escape("unable to resolve the additional pieces");

      leave();
    }

  }
}

#endif
