#ifndef NUCLEUS_NEUTRON_TOKEN_HXX
# define NUCLEUS_NEUTRON_TOKEN_HXX

# include <elle/cryptography/PrivateKey.hh>
# include <elle/cryptography/PublicKey.hh>
# include <elle/cryptography/SecretKey.hh>
# include <elle/cryptography/Code.hh>

# include <nucleus/Exception.hh>

# include <elle/idiom/Open.hh>

namespace nucleus
{
  namespace neutron
  {

//
// ---------- construction ----------------------------------------------------
//

    template <typename T>
    Token::Token(elle::cryptography::PublicKey const& K,
                 T const& secret):
      code(nullptr)
    {
      // XXX[la methode update devrait etre viree et un token devrait etre
      //     cree via le constructeur a chaque fois]
      if (this->Update(secret, K) == elle::Status::Error)
        throw Exception("unable to construct the token");
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates or update the token.
    ///
    template <typename T>
    elle::Status Token::Update(T const& secret,
                               elle::cryptography::PublicKey const& K)
    {
      delete this->code;

      // if the secret key is null, reinitialize to the default null token.
      if (secret == T::Null)
        {
          assert(false && "[XXX] another method should be introduced");

          this->code = nullptr;
        }
      else
        {
          // allocate a new code.
          this->code = new elle::cryptography::Code;

          // encrypt the given secret with the given public key.
          if (K.Encrypt(secret, *this->code) == elle::Status::Error)
            escape("unable to encrypt the secret");
        }

      return elle::Status::Ok;
    }

    ///
    /// this method extracts the secret key from the token.
    ///
    template <typename T>
    elle::Status Token::Extract(elle::cryptography::PrivateKey const&  k,
                                T& secret) const
    {
      // check the code.
      if (this->code == nullptr)
        escape("unable to retrieve the secret out of a null token");

      // decrypt the code.
      if (k.Decrypt(*this->code, secret) == elle::Status::Error)
        escape("unable to decrypt the token");

      return elle::Status::Ok;
    }

  }
}

//
// ---------- serialize -------------------------------------------------------
//

# include <cassert>

# include <elle/serialize/Pointer.hh>

ELLE_SERIALIZE_SIMPLE(nucleus::neutron::Token,
                      archive,
                      value,
                      version)
{
  assert(version == 0);

  archive & elle::serialize::pointer(value.code);
}

#endif
