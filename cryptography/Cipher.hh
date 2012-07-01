#ifndef ELLE_CRYPTOGRAPHY_CIPHER_HH
# define ELLE_CRYPTOGRAPHY_CIPHER_HH

# include <elle/types.hh>

# include <elle/standalone/Region.hh>

# include <elle/radix/Object.hh>

# include <elle/idiom/Open.hh>

namespace elle
{
  namespace cryptography
  {

    ///
    /// this class represents a symmetrically ciphered text.
    ///
    class Cipher:
      public radix::Object
    {
    public:
      //
      // constants
      //
      static const Cipher               Null;

      //
      // constructors & destructors
      //
      Cipher();

      //
      // interfaces
      //

      // object
      declare(Cipher);
      Boolean           operator==(const Cipher&) const;

      // dumpable
      Status            Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      standalone::Region region;
    };

  }
}

#include <elle/cryptography/Cipher.hxx>

#endif
