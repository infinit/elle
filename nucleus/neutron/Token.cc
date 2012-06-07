
#include <elle/cryptography/SecretKey.hh>

#include <elle/cryptography/Code.hh>
#include <elle/cryptography/SecretKey.hh>
#include <elle/cryptography/PublicKey.hh>
#include <elle/cryptography/PrivateKey.hh>

#include <nucleus/neutron/Token.hh>

#include <elle/idiom/Open.hh>

namespace nucleus
{
  namespace neutron
  {

//
// ---------- definition ------------------------------------------------------
//

    ///
    /// this macro defines an empty token.
    ///
    const Token                 Token::Null;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor
    ///
    Token::Token():
      code(NULL)
    {
    }

    ///
    /// copy constructor.
    ///
    Token::Token(const Token&                                   token):
      Object(token)
    {
      // copy the code, if present.
      if (token.code != NULL)
        {
          // duplicate the code.
          this->code = new elle::cryptography::Code(*token.code);
        }
      else
        this->code = NULL;
    }

    ///
    /// destructor.
    ///
    Token::~Token()
    {
      // release the code.
      if (this->code != NULL)
        delete this->code;
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates or update the token.
    ///
    elle::Status        Token::Update(elle::cryptography::SecretKey const&    key,
                                      elle::cryptography::PublicKey const&    K)
    {
      // delete the previous code.
      if (this->code != NULL)
        delete this->code;

      // if the secret key is null, reinitialize to the default null token.
      if (key == elle::cryptography::SecretKey::Null)
        {
          this->code = NULL;
        }
      else
        {
          // allocate a new code.
          this->code = new elle::cryptography::Code;

          // encrypt the given secret key with the given public key.
          if (K.Encrypt(key, *this->code) == elle::Status::Error)
            escape("unable to encrypt the key");
        }

      return elle::Status::Ok;
    }

    ///
    /// this method extracts the secret key from the token.
    ///
    elle::Status        Token::Extract(elle::cryptography::PrivateKey const&  k,
                                       elle::cryptography::SecretKey&         key) const
    {
      // check the code.
      if (this->code == NULL)
        escape("unable to retrieve the key out of a null token");

      // decrypt the code.
      if (k.Decrypt(*this->code, key) == elle::Status::Error)
        escape("unable to decrypt the token's content");

      return elle::Status::Ok;
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this operator compares two objects.
    ///
    elle::Boolean       Token::operator==(const Token&          element) const
    {
      // check if the objects are the same.
      if (this == &element)
        return true;

      // compare the code.
      if ((this->code == NULL) || (element.code == NULL))
        {
          if (this->code != element.code)
            return false;
        }
      else
        {
          if (*this->code != *element.code)
            return false;
        }

      return true;
    }

    ///
    /// this macro-function call generates the object.
    ///
    embed(Token, _());

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps an block object.
    ///
    elle::Status        Token::Dump(elle::Natural32     margin) const
    {
      elle::String      alignment(margin, ' ');

      std::cout << alignment << "[Token] " << std::endl;

      // dump the code.
      if (this->code != NULL)
        {
          if (this->code->Dump(margin + 2) == elle::Status::Error)
            escape("unable to dump the parent Code class");
        }
      else
        {
          std::cout << alignment << elle::Dumpable::Shift
                    << "[Code] " << elle::none << std::endl;
        }

      return elle::Status::Ok;
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the block object.
    ///
    //elle::Status        Token::Serialize(elle::Archive&         archive) const
    //{
    //  // serialize the code.
    //  if (this->code != NULL)
    //    {
    //      if (archive.Serialize(*this->code) == elle::Status::Error)
    //        escape("unable to serialize the code");
    //    }
    //  else
    //    {
    //      // serialize 'none'.
    //      if (archive.Serialize(elle::none) == elle::Status::Error)
    //        escape("unable to serialize 'none'");
    //    }

    //  return elle::Status::Ok;
    //}

    ///
    /// this method extracts the block object.
    ///
    //elle::Status        Token::Extract(elle::Archive&           archive)
    //{
    //  elle::Archive::Type       type;

    //  // fetch the next element's type.
    //  if (archive.Fetch(type) == elle::Status::Error)
    //    escape("unable to fetch the next element's type");

    //  if (type == elle::Archive::TypeNull)
    //    {
    //      // nothing to do, keep the code to NULL.
    //      if (archive.Extract(elle::none) == elle::Status::Error)
    //        escape("unable to extract null");
    //    }
    //  else
    //    {
    //      // allocate a code.
    //      this->code = new elle::cryptography::Code;

    //      // extract the code.
    //      if (archive.Extract(*this->code) == elle::Status::Error)
    //        escape("unable to extract the code");
    //    }

    //  return elle::Status::Ok;
    //}

  }
}
