#ifndef INFINIT_CRYPTOGRAPHY_SIGNATURE_HH
#define INFINIT_CRYPTOGRAPHY_SIGNATURE_HH

#include <elle/types.hh>

#include <elle/standalone/Region.hh>

#include <elle/radix/Object.hh>

#include <elle/idiom/Open.hh>

namespace infinit
{
  namespace cryptography
  {

    ///
    /// this class represents an asymmetrically encrypted text.
    ///
    class Signature:
      public elle::radix::Object
    {
    public:
      //
      // constants
      //
      static const Signature            Null;

      //
      // constructors & destructors
      //
      Signature();

      //
      // interfaces
      //

      // object
      declare(Signature);
      elle::Boolean           operator==(const Signature&) const;

      // dumpable
      elle::Status            Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
      elle::standalone::Region region;
    };

  }
}

#include <cryptography/Signature.hxx>

#endif
