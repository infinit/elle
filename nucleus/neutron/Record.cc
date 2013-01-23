#include <nucleus/neutron/Record.hh>
#include <nucleus/Exception.hh>

namespace nucleus
{
  namespace neutron
  {
//
// ---------- Record ----------------------------------------------------------
//

    /*-------------.
    | Construction |
    `-------------*/

    Record::Record():
      _valid(nullptr),
      _footprint(0)
    {
    }

    Record::Record(Subject const& subject,
                   Permissions permissions,
                   Token const& token):
      _type(Type::valid),
      _valid(new Valid(subject, permissions, token)),
      _footprint(0)
    {
      // Compute the initial footprint.
      this->_footprint = elle::serialize::footprint(*this);
    }

    Record::Record(Subject const& subject,
                   Permissions permissions,
                   Token const* token):
      Record(subject, permissions,
             token != nullptr ? *token : Token::null())
    {
    }

    Record::Record(Record const& other):
      _type(other._type),
      _valid(nullptr),
      _footprint(other._footprint)
    {
      if (other._valid != nullptr)
        this->_valid =
          new Valid(
            other._valid->subject(),
            other._valid->permissions(),
            other._valid->token());
    }

    Record::Record(Type const type):
      _type(type),
      _valid(nullptr),
      _footprint(0)
    {
      switch (this->_type)
        {
        case Type::null:
          {
            // Nothing to do; this is the right way to construct such special
            // records.
            break;
          }
        case Type::valid:
          {
            throw Exception("valid records cannot be built through this "
                            "constructor");
          }
        default:
          throw Exception("unknown record type '%s'", this->_type);
        }

      // Compute the initial footprint.
      this->_footprint = elle::serialize::footprint(*this);
    }

    ELLE_SERIALIZE_CONSTRUCT_DEFINE(Record)
    {
      this->_valid = nullptr;
    }

    Record::~Record()
    {
      delete this->_valid;
    }

    /*--------.
    | Methods |
    `--------*/

    Subject const&
    Record::subject() const
    {
      ELLE_ASSERT(this->_type == Type::valid);
      ELLE_ASSERT(this->_valid != nullptr);

      return (this->_valid->subject());
    }

    Permissions
    Record::permissions() const
    {
      ELLE_ASSERT(this->_type == Type::valid);
      ELLE_ASSERT(this->_valid != nullptr);

      return (this->_valid->permissions());
    }

    void
    Record::permissions(Permissions const permissions)
    {
      ELLE_ASSERT(this->_type == Type::valid);
      ELLE_ASSERT(this->_valid != nullptr);

      // Substract the current permissions' footprint.
      ELLE_ASSERT(this->_footprint >=
                  elle::serialize::footprint(this->_valid->permissions()));
      this->_footprint -=
        elle::serialize::footprint(this->_valid->permissions());

      // Update the permissions.
      this->_valid->permissions(permissions);

      // Add the new permissions' footprint.
      this->_footprint += elle::serialize::footprint(permissions);
    }

    Token const&
    Record::token() const
    {
      ELLE_ASSERT(this->_type == Type::valid);
      ELLE_ASSERT(this->_valid != nullptr);

      return (this->_valid->token());
    }

    void
    Record::token(Token const& token)
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
    Record::operator ==(Record const& other) const
    {
      if (this == &other)
        return (true);

      if (this->_type != other._type)
        return (false);

      if (this->_type == Type::valid)
        throw Exception("it does not make any sense to compare valid records");

      return (true);
    }

    /*---------.
    | Dumpable |
    `---------*/

    elle::Status
    Record::Dump(const elle::Natural32 margin) const
    {
      elle::String alignment(margin, ' ');

      switch (this->_type)
        {
        case Type::null:
          {
            std::cout << alignment << "[Record] " << "none" << std::endl;

            break;
          }
        case Type::valid:
          {
            ELLE_ASSERT(this->_valid != nullptr);

            std::cout << alignment << "[Record]" << std::endl;

            if (this->_valid->subject().Dump(margin + 2) == elle::Status::Error)
              escape("unable to dump the subject");

            std::cout << alignment << elle::io::Dumpable::Shift
                      << "[Permissions] " << std::dec
                      << this->_valid->permissions() << std::endl;

            if (this->_valid->token().Dump(margin + 2) == elle::Status::Error)
              escape("unable to dump the token");

            break;
          }
        default:
          throw Exception("unknown record type '%s'", this->_type);
        }

      return elle::Status::Ok;
    }

    /*----------.
    | Printable |
    `----------*/

    void
    Record::print(std::ostream& stream) const
    {
      switch (this->_type)
        {
        case Type::null:
          {
            stream << "null";
            break;
          }
        case Type::valid:
          {
            ELLE_ASSERT(this->_valid != nullptr);

            stream << this->_valid->subject()
                   << ", "
                   << this->_valid->permissions();

            break;
          }
        default:
          throw Exception("unknown record type '%s'", this->_type);
        }
    }

    /*----------.
    | Rangeable |
    `----------*/

    Subject const&
    Record::symbol() const
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

    Record::Valid::Valid():
      _token(nullptr)
    {
    }

    Record::Valid::Valid(Subject const& subject,
                         Permissions const permissions,
                         Token const& token):
      _subject(subject),
      _permissions(permissions),
      _token(new Token(token))
    {
    }

    Record::Valid::~Valid()
    {
      delete this->_token;
    }

    /*--------.
    | Methods |
    `--------*/

    Token const&
    Record::Valid::token() const
    {
      ELLE_ASSERT(this->_token != nullptr);

      return (*this->_token);
    }

    void
    Record::Valid::token(Token const& token)
    {
      delete this->_token;
      this->_token = nullptr;
      this->_token = new Token(token);
    }

    /*----------.
    | Printable |
    `----------*/

    void
    Record::Valid::print(std::ostream& stream) const
    {
      stream << this->_subject << ", " << this->_permissions;
    }

//
// ---------- Other -----------------------------------------------------------
//

    /*---------------.
    | Static Methods |
    `---------------*/

    Record const&
    Record::null()
    {
      static Record record(Record::Type::null);

      return (record);
    }

    /*----------.
    | Operators |
    `----------*/

    std::ostream&
    operator <<(std::ostream& stream,
                Record::Type const type)
    {
      switch (type)
        {
        case Record::Type::null:
          {
            stream << "null";
            break;
          }
        case Record::Type::valid:
          {
            stream << "valid";
            break;
          }
        default:
          {
            throw Exception("unknown record type: '%s'",
                            static_cast<int>(type));
          }
        }

      return (stream);
    }
  }
}
