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

    /*---------------.
    | Static Methods |
    `---------------*/

    Token const&
    Token::null()
    {
      static Token token(Token::Type::null);

      return (token);
    }

    /*-------------.
    | Construction |
    `-------------*/

    Token::Token():
      _valid(nullptr)
    {
    }

    Token::Token(const Token& other):
      _type(other._type),
      _valid(nullptr)
    {
      if (other._valid != nullptr)
        this->_valid = new Token::Valid(other._valid->code());
    }

    Token::Token(Type const type):
      _type(type),
      _valid(nullptr)
    {
      switch (this->_type)
        {
        case Type::null:
          {
            // Nothing to do; this is the right way to construct such special
            // tokens.
            break;
          }
        case Type::valid:
          {
            throw Exception("valid tokens cannot be built through this "
                            "constructor");
          }
        default:
          throw Exception("unknown token type '%s'", this->_type);
        }
    }

    Token::~Token()
    {
      delete this->_valid;
    }

    Token::Valid::Valid()
    {
    }

    Token::Valid::Valid(elle::cryptography::Code const& code):
      _code(code)
    {
    }

    /*----------.
    | Operators |
    `----------*/

    elle::Boolean
    Token::operator ==(Token const& other) const
    {
      if (this == &other)
        return true;

      if (this->_type != other._type)
        return (false);

      if (this->_type == Type::valid)
        {
          ELLE_ASSERT(this->_valid != nullptr);
          ELLE_ASSERT(other._valid != nullptr);

          if ((this->_valid->code() != other._valid->code()))
            return (false);
        }

      return (true);
    }

    /*---------.
    | Dumpable |
    `---------*/

    elle::Status        Token::Dump(elle::Natural32     margin) const
    {
      elle::String      alignment(margin, ' ');

      switch (this->_type)
        {
        case Type::null:
          {
            std::cout << alignment << "[Token] " << elle::none << std::endl;

            break;
          }
        case Type::valid:
          {
            ELLE_ASSERT(this->_valid != nullptr);

            std::cout << alignment << "[Token] " << std::endl;

            if (this->_valid->code().Dump(margin + 2) == elle::Status::Error)
              escape("unable to dump the parent Code class");
          }
        default:
          throw Exception("unknown token type '%s'", this->_type);
        }

      return elle::Status::Ok;
    }

    /*----------.
    | Printable |
    `----------*/

    void
    Token::print(std::ostream& stream) const
    {
      switch (this->_type)
        {
        case Type::null:
          {
            stream << "token(null)";
            break;
          }
        case Type::valid:
          {
            ELLE_ASSERT(this->_valid != nullptr);

            stream << "code("
                   << this->_valid->code()
                   << ")";

            break;
          }
        default:
          throw Exception("unknown token type '%s'", this->_type);
        }
    }

  }
}
