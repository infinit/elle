#ifndef ELLE_IO_PIECE_HH
# define ELLE_IO_PIECE_HH

# include <elle/types.hh>
# include <elle/operator.hh>

namespace elle
{
  namespace io
  {

    ///
    /// this class represents a variable piece of a pattern-based path.
    ///
    class Piece
    {
    public:
      //
      // constructors & destructors
      //
      Piece();
      Piece(const String&,
            const String&);
      Piece(Piece const& other) = default;

      //
      // interfaces
      //

      ELLE_OPERATOR_NO_ASSIGNMENT(Piece);

      Boolean           operator==(const Piece&) const;

      // dumpable
      Status            Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      const String      name;
      const String      value;
    };

  }
}

#endif
