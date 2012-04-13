//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [sat mar 19 13:41:49 2011]
//

#ifndef ELLE_CRYPTOGRAPHY_SEED_HH
#define ELLE_CRYPTOGRAPHY_SEED_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/types.hh>

#include <elle/radix/Status.hh>
#include <elle/radix/Object.hh>

#include <elle/package/Archive.hh>

#include <elle/cryptography/PublicKey.hh>
#include <elle/cryptography/PrivateKey.hh>

namespace elle
{

  using namespace radix;
  using namespace package;

  namespace cryptography
  {

//
// ---------- forward declarations --------------------------------------------
//

    ///
    /// the PublicKey and PrivateKey must be forward declared to prevent
    /// conflicts.
    ///
    class PublicKey;
    class PrivateKey;

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a seed which can especially be used for
    /// rotating keys.
    ///
    class Seed:
      public Object
    {
    public:
      //
      // constants
      //
      static const Natural32            Length;

      //
      // methods
      //
      Status            Generate();
      Status            Generate(const Natural32);

      Status            Rotate(const PrivateKey&,
                               Seed&) const;
      Status            Derive(const PublicKey&,
                               Seed&) const;

      //
      // interfaces
      //

      // object
      declare(Seed);
      Boolean           operator==(const Seed&) const;

      // dumpable
      Status            Dump(const Natural32 = 0) const;

      // archivable
      Status            Serialize(Archive&) const;
      Status            Extract(Archive&);

      //
      // attributes
      //
      Region            region;
    };

  }
}

#endif
