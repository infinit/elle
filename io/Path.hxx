#ifndef ELLE_IO_PATH_HXX
# define ELLE_IO_PATH_HXX

# include <elle/Exception.hh>
# include <elle/idiom/Open.hh>

# include <stdexcept>

namespace elle
{
  namespace io
  {

//
// ---------- variadic templates ----------------------------------------------
//

    template <typename T,
              typename... TT>
    Path::Path(Pattern const& pattern,
               T const& piece,
               TT const&... pieces)
    {
      if (this->Create(pattern) == Status::Error)
        throw std::runtime_error("unable to create the path based "
                                 "on a pattern");

      if (this->Complete(piece) == Status::Error)
        throw std::runtime_error("unable to complete the path");

      if (this->Complete(pieces...) == Status::Error)
        throw std::runtime_error("unable to complete the path");
    }

    ///
    /// this template resolves a single piece of the path's pattern.
    ///
    template <typename T>
    Status              Path::Complete(T const& piece)
    {
      // resolve the piece.
      if (this->Complete(piece.name, piece.value) == Status::Error)
        escape("unable to resolve the piece");

      return Status::Ok;
    }

    ///
    /// this method resolves the given set of pieces for the path's pattern.
    ///
    template <typename T,
              typename... TT>
    Status              Path::Complete(T const& piece,
                                       TT const&... pieces)
    {
      // resolve the given piece.
      if (this->Complete(piece) == Status::Error)
        escape("unable to resolve the first piece");

      // resolve the additional pieces.
      if (this->Complete(pieces...) == Status::Error)
        escape("unable to resolve the additional pieces");

      return Status::Ok;
    }

  }
}

#endif
