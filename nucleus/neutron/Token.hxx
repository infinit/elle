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

    /*-------------.
    | Construction |
    `-------------*/

    template <typename T>
    Token::Token(T const& secret,
                 elle::cryptography::PublicKey const& K):
      _type(Type::valid),
      _valid(nullptr)
    {
      elle::cryptography::Code code;

      // Encrypt the given secret with the given public key.
      if (K.Encrypt(secret, code) == elle::Status::Error)
        escape("unable to encrypt the secret");

      // XXX[this could be improved by putting it directly in the
      //     initialization list]
      this->_valid = new Valid(code);
    }

    /*--------.
    | Methods |
    `--------*/

    template <typename T>
    T
    Token::extract(elle::cryptography::PrivateKey const& k) const
    {
      T secret;

      ELLE_ASSERT(this->_valid != nullptr);

      // Decrypt the code, revealing the secret information.
      if (k.Decrypt(this->_valid->code(), secret) == elle::Status::Error)
        escape("unable to decrypt the token");

      return (secret);
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

  archive & value._type;
  switch (value._type)
    {
    case nucleus::neutron::Token::Type::null:
      {
        break;
      }
    case nucleus::neutron::Token::Type::valid:
      {
        archive & elle::serialize::alive_pointer(value._valid);

        break;
      }
    default:
      throw Exception("unknown token type '%s'", value._type);
    }
}

ELLE_SERIALIZE_SIMPLE(nucleus::neutron::Token::Valid,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);

  archive & value._code;
}

#endif
