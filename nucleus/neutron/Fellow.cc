#include <nucleus/neutron/Fellow.hh>

namespace nucleus
{
  namespace neutron
  {
//
// ---------- Fellow ----------------------------------------------------------
//

    /*-------------.
    | Construction |
    `-------------*/

    Fellow::Fellow():
      _valid(nullptr),
      _footprint(0)
    {
    }

    Fellow::Fellow(Subject const& subject,
                   Token const& token):
      _type(Type::valid),
      _valid(new Valid(subject, token)),
      _footprint(0)
    {
      // Compute the initial footprint.
      this->_footprint = elle::serialize::footprint(*this);
    }

    Fellow::Fellow(Fellow const& other):
      _type(other._type),
      _valid(nullptr),
      _footprint(other._footprint)
    {
      if (other._valid != nullptr)
        this->_valid = new Valid(other._valid->subject(),
                                 other._valid->token());
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

      // Compute the initial footprint.
      this->_footprint = elle::serialize::footprint(*this);
    }

    ELLE_SERIALIZE_CONSTRUCT_DEFINE(Fellow)
    {
      this->_valid = nullptr;
    }

    Fellow::~Fellow()
    {
      delete this->_valid;
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

      return (this->_valid->token());
    }

    void
    Fellow::token(Token const& token)
    {
      ELLE_ASSERT(this->_type == Type::valid);
      ELLE_ASSERT(this->_valid != nullptr);

      // Substract the current token's footprint.
      ELLE_ASSERT(this->_footprint >=
                  elle::serialize::footprint(this->_valid->token()));
      this->_footprint -=
        elle::serialize::footprint(this->_valid->token());

      // Update the token.
      this->_valid->token(token);

      // Add the new token's footprint.
      this->_footprint += elle::serialize::footprint(token);
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
            std::cout << alignment << "[Fellow] " << "none" << std::endl;

            break;
          }
        case Type::valid:
          {
            ELLE_ASSERT(this->_valid != nullptr);

            std::cout << alignment << "[Fellow]" << std::endl;

            if (this->_valid->subject().Dump(margin + 2) == elle::Status::Error)
              escape("unable to dump the subject");

            if (this->_valid->token().Dump(margin + 2) == elle::Status::Error)
              escape("unable to dump the token");

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
    Fellow::symbol() const
    {
      ELLE_ASSERT(this->_type == Type::valid);
      ELLE_ASSERT(this->_valid != nullptr);

      return (this->_valid->subject());
    }

//
// ---------- Valid -----------------------------------------------------------
//

    /*-------------.
    | Construction |
    `-------------*/

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

    Token const&
    Fellow::Valid::token() const
    {
      ELLE_ASSERT(this->_token != nullptr);

      return (*this->_token);
    }

    void
    Fellow::Valid::token(Token const& token)
    {
      delete this->_token;
      this->_token = nullptr;
      this->_token = new Token(token);
    }

//
// ---------- Other -----------------------------------------------------------
//

    /*---------------.
    | Static Methods |
    `---------------*/

    Fellow const&
    Fellow::null()
    {
      static Fellow fellow(Fellow::Type::null);

      return (fellow);
    }

    /*----------.
    | Operators |
    `----------*/

    std::ostream&
    operator <<(std::ostream& stream,
                Fellow::Type const type)
    {
      switch (type)
        {
        case Fellow::Type::null:
          {
            stream << "null";
            break;
          }
        case Fellow::Type::valid:
          {
            stream << "valid";
            break;
          }
        default:
          {
            throw Exception("unknown fellow type: '%s'",
                            static_cast<int>(type));
          }
        }

      return (stream);
    }
  }
}
