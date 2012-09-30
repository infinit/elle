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

    Token::Token(const Token& other)
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
// ---------- operators -------------------------------------------------------
//

    elle::Boolean
    Token::operator ==(Token const& other) const
    {
      // check if the objects are the same.
      if (this == &other)
        return true;

      // compare the code.
      if ((this->_code == nullptr) || (other._code == nullptr))
        {
          if (this->_code != other._code)
            return false;
        }
      else
        {
          if (*this->_code != *other._code)
            return false;
        }

      return true;
    }

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

//
// ---------- printable -------------------------------------------------------
//

    void
    Token::print(std::ostream& stream) const
    {
      stream << "code("
             << *this->_code
             << ")";
    }

  }
}
