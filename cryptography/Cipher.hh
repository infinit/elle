#ifndef INFINIT_CRYPTOGRAPHY_CIPHER_HH
# define INFINIT_CRYPTOGRAPHY_CIPHER_HH

# include <elle/types.hh>

# include <elle/standalone/Region.hh>

# include <elle/radix/Object.hh>

# include <elle/idiom/Open.hh>

namespace infinit
{
  namespace cryptography
  {

    ///
    /// this class represents a symmetrically ciphered text.
    ///
    class Cipher:
      public elle::radix::Object
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
      elle::Boolean           operator==(const Cipher&) const;

      // dumpable
      elle::Status            Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
      elle::standalone::Region region;
    };

  }
}

#include <cryptography/Cipher.hxx>

#endif
