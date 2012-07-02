#ifndef ELLE_IO_PIECE_HH
# define ELLE_IO_PIECE_HH

# include <elle/types.hh>

# include <elle/radix/Object.hh>

# include <elle/idiom/Open.hh>

namespace elle
{
  namespace io
  {

    ///
    /// this class represents a variable piece of a pattern-based path.
    ///
    class Piece:
      public radix::Object
    {
    public:
      //
      // constructors & destructors
      //
      Piece();
      Piece(const String&,
            const String&);

      //
      // interfaces
      //

      // object
      declare(Piece);
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
