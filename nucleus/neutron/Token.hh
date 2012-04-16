//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien quintard   [fri jul 24 16:28:56 2009]
//

#ifndef NUCLEUS_NEUTRON_TOKEN_HH
#define NUCLEUS_NEUTRON_TOKEN_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/types.hh>

namespace nucleus
{
  namespace neutron
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// a token is a secret key enabling a user to access encrypted
    /// data. however, in order to allow only the proper user to
    /// use this key, it is encrypted with the user's public key.
    ///
    class Token:
      public elle::Object
    {
    public:
      //
      // constants
      //
      static const Token                Null;

      //
      // constructors & destructors
      //
      Token();
      Token(const Token&);
      ~Token();

      //
      // methods
      //
      elle::Status      Update(elle::cryptography::SecretKey&,
                               elle::cryptography::PublicKey const&);
      elle::Status      Extract(elle::cryptography::PrivateKey const&,
                                elle::cryptography::SecretKey&) const;

      //
      // interfaces
      //

      // object
      declare(Token);
      elle::Boolean     operator==(const Token&) const;

      // dumpable
      elle::Status      Dump(const elle::Natural32 = 0) const;

      //// archivable
      //elle::Status      Serialize(elle::Archive&) const;
      //elle::Status      Extract(elle::Archive&);

      //
      // attributes
      //
      elle::cryptography::Code*       code;
    };

  }
}

#endif
