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

    const Token Token::Null;

//
// ---------- constructors & destructors --------------------------------------
//

    Token::Token():
      _code(nullptr)
    {
    }

    Token::Token(const Token& other):
      Object(other)
    {
      if (other._code != nullptr)
        {
          // Duplicate the code.
          this->_code = new elle::cryptography::Code(*other.code());
        }
      else
        this->_code = nullptr;
    }

    Token::~Token()
    {
      delete this->_code;
    }

//
// ---------- methods ---------------------------------------------------------
//

    elle::cryptography::Code const*
    Token::code() const
    {
      return (this->_code);
    }

//
// ---------- object ----------------------------------------------------------
//

    elle::Boolean       Token::operator==(const Token&          element) const
    {
      // check if the objects are the same.
      if (this == &element)
        return true;

      // compare the code.
      if ((this->_code == nullptr) || (element.code() == nullptr))
        {
          if (this->_code != element.code())
            return false;
        }
      else
        {
          if (*this->_code != *element.code())
            return false;
        }

      return true;
    }

    embed(Token, _());

//
// ---------- dumpable --------------------------------------------------------
//

    elle::Status        Token::Dump(elle::Natural32     margin) const
    {
      elle::String      alignment(margin, ' ');

      std::cout << alignment << "[Token] " << std::endl;

      if (this->_code != nullptr)
        {
          if (this->_code->Dump(margin + 2) == elle::Status::Error)
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
