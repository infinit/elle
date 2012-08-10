#include <nucleus/neutron/Token.hh>

#include <elle/cryptography/SecretKey.hh>
#include <elle/cryptography/Code.hh>
#include <elle/cryptography/PublicKey.hh>
#include <elle/cryptography/PrivateKey.hh>

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
      code(nullptr)
    {
    }

    ///
    /// copy constructor.
    ///
    Token::Token(const Token&                                   token):
      Object(token)
    {
      // copy the code, if present.
      if (token.code != nullptr)
        {
          // duplicate the code.
          this->code = new elle::cryptography::Code(*token.code);
        }
      else
        this->code = nullptr;
    }

    ///
    /// destructor.
    ///
    Token::~Token()
    {
      // release the code.
      if (this->code != nullptr)
        delete this->code;
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
      if ((this->code == nullptr) || (element.code == nullptr))
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
      if (this->code != nullptr)
        {
          if (this->code->Dump(margin + 2) == elle::Status::Error)
            escape("unable to dump the parent Code class");
        }
      else
        {
          std::cout << alignment << elle::io::Dumpable::Shift
                    << "[Code] " << elle::none << std::endl;
        }

      return elle::Status::Ok;
    }

  }
}
