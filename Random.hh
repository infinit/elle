#ifndef ELLE_CRYPTOGRAPHY_RANDOM_HH
# define ELLE_CRYPTOGRAPHY_RANDOM_HH

# include <elle/types.hh>

# include <elle/standalone/fwd.hh>

namespace elle
{

  namespace cryptography
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class provides functionalities for generating random values.
    ///
    /// \todo XXX note that the current implementation is Linux-specific
    ///           since it reads directly from '/dev/random'.
    class Random
    {
    public:
      //
      // constants
      //
      static const Natural32            Bits = 1024;
      static const Natural32            Length = 128;
      static const Natural32            Size = 512;

      //
      // static method
      //
      static Status     Initialize();
      static Status     Clean();

      static Status     Generate(Boolean&);
      static Status     Generate(Character&);
      static Status     Generate(Real&);
      static Status     Generate(Integer8&);
      static Status     Generate(Integer16&);
      static Status     Generate(Integer32&);
      static Status     Generate(Integer64&);
      static Status     Generate(Natural8&);
      static Status     Generate(Natural16&);
      static Status     Generate(Natural32&);
      static Status     Generate(Natural64&);
      static Status     Generate(Large&,
                                 const Natural32 = Bits);
      static Status     Generate(String&,
                                 const Natural32 = Length);
      static Status     Generate(standalone::Region&,
                                 const Natural32 = Size);
    };

  }
}

#endif
