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
      _code(nullptr)
    {
      // XXX[la methode update devrait etre viree et un token devrait etre
      //     cree via le constructeur a chaque fois OU renommer Update()]
      if (this->Update(K, secret) == elle::Status::Error)
        throw Exception("unable to construct the token");
    }

//
// ---------- methods ---------------------------------------------------------
//

    template <typename T>
    elle::Status Token::Update(elle::cryptography::PublicKey const& K,
                               T const& secret)
    {
      delete this->_code;

      // allocate a new code.
      this->_code = new elle::cryptography::Code;

      // encrypt the given secret with the given public key.
      if (K.Encrypt(secret, *this->_code) == elle::Status::Error)
        escape("unable to encrypt the secret");

      return elle::Status::Ok;
    }

    template <typename T>
    elle::Status Token::Extract(elle::cryptography::PrivateKey const&  k,
                                T& secret) const
    {
      if (this->_code == nullptr)
        escape("unable to retrieve the secret out of a null token");

      // Decrypt the code, revealing the secret information.
      if (k.Decrypt(*this->_code, secret) == elle::Status::Error)
        escape("unable to decrypt the token");

      return elle::Status::Ok;
    }

  }
}

//
// ---------- serialize -------------------------------------------------------
//


# include <elle/serialize/Pointer.hh>

ELLE_SERIALIZE_SIMPLE(nucleus::neutron::Token,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);

  archive & elle::serialize::pointer(value._code);
}

#endif
