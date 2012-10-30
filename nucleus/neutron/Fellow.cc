#include <nucleus/neutron/Fellow.hh>

#include <elle/idiom/Open.hh>

namespace nucleus
{
  namespace neutron
  {

    /*---------------.
    | Static Methods |
    `---------------*/

    Fellow const&
    Fellow::null()
    {
      static Fellow fellow(Fellow::Type::null);

      return (fellow);
    }

    /*-------------.
    | Construction |
    `-------------*/

    Fellow::Fellow():
      _valid(nullptr)
    {
    }

    Fellow::Fellow(Subject const& subject,
                   Token const& token):
      _type(Type::valid),
      _valid(new Valid(subject, token))
    {
    }

    Fellow::Fellow(Fellow const& other):
      _type(other._type),
      _valid(nullptr)
    {
      if (other._valid != nullptr)
        {
          ELLE_ASSERT(other._valid->token() != nullptr);

          this->_valid = new Valid(other._valid->subject(),
                                   *other._valid->token());
        }
    }

    Fellow::Fellow(Type const type):
      _type(type),
      _valid(nullptr)
    {
      switch (this->_type)
        {
        case Type::null:
          {
            // Nothing to do; this is the right way to construct such special
            // fellows.
            break;
          }
        case Type::valid:
          {
            throw Exception("valid fellows cannot be built through this "
                            "constructor");
          }
        default:
          throw Exception("unknown fellow type '%s'", this->_type);
        }
    }

    Fellow::~Fellow()
    {
      delete this->_valid;
    }

    Fellow::Valid::Valid():
      _token(nullptr)
    {
    }

    Fellow::Valid::Valid(Subject const& subject,
                         Token const& token):
      _subject(subject),
      _token(new Token(token))
    {
    }

    Fellow::Valid::~Valid()
    {
      delete this->_token;
    }

    /*--------.
    | Methods |
    `--------*/

    Subject const&
    Fellow::subject() const
    {
      ELLE_ASSERT(this->_type == Type::valid);
      ELLE_ASSERT(this->_valid != nullptr);

      return (this->_valid->subject());
    }

    Token const&
    Fellow::token() const
    {
      ELLE_ASSERT(this->_type == Type::valid);
      ELLE_ASSERT(this->_valid != nullptr);
      ELLE_ASSERT(this->_valid->token() != nullptr);

      // A const reference is returned because fellows are always
      // supposed to contain a valid token.
      return (*this->_valid->token());
    }

    void
    Fellow::token(Token const& token)
    {
      ELLE_ASSERT(this->_type == Type::valid);
      ELLE_ASSERT(this->_valid != nullptr);

      this->_valid->token(token);
    }

    void
    Fellow::Valid::token(Token const& token)
    {
      delete this->_token;
      this->_token = new Token(token);
    }

    /*----------.
    | Operators |
    `----------*/

    elle::Boolean
    Fellow::operator ==(Fellow const& other) const
    {
      if (this == &other)
        return (true);

      if (this->_type != other._type)
        return (false);

      if (this->_type == Type::valid)
        throw Exception("it does not make any sense to compare valid fellows");

      return (true);
    }

    /*---------.
    | Dumpable |
    `---------*/

    elle::Status
    Fellow::Dump(const elle::Natural32 margin) const
    {
      elle::String alignment(margin, ' ');

      switch (this->_type)
        {
        case Type::null:
          {
            std::cout << alignment << "[Fellow] " << elle::none << std::endl;

            break;
          }
        case Type::valid:
          {
            ELLE_ASSERT(this->_valid != nullptr);

            std::cout << alignment << "[Fellow]" << std::endl;

            if (this->_valid->subject().Dump(margin + 2) == elle::Status::Error)
              escape("unable to dump the subject");

            if (this->_valid->token() != nullptr)
              {
                if (this->_valid->token()->Dump(margin + 2) == elle::Status::Error)
                  escape("unable to dump the token");
              }
            else
              std::cout << alignment << elle::io::Dumpable::Shift
                        << "[Token] " << elle::none << std::endl;

            break;
          }
        default:
          throw Exception("unknown fellow type '%s'", this->_type);
        }

      return (elle::Status::Ok);
    }

    /*----------.
    | Printable |
    `----------*/

    void
    Fellow::print(std::ostream& stream) const
    {
      switch (this->_type)
        {
        case Type::null:
          {
            stream << "fellow(null)";
            break;
          }
        case Type::valid:
          {
            ELLE_ASSERT(this->_valid != nullptr);

            stream << "fellow("
                   << this->_valid->subject()
                   << ")";

            break;
          }
        default:
          throw Exception("unknown fellow type '%s'", this->_type);
        }
    }

    /*----------.
    | Rangeable |
    `----------*/

    Subject const&
    Fellow::symbol()
    {
      ELLE_ASSERT(this->_type == Type::valid);
      ELLE_ASSERT(this->_valid != nullptr);

      return (this->_valid->subject());
    }

  }
}
