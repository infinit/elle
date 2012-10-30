#include <nucleus/neutron/Record.hh>
#include <nucleus/Exception.hh>

#include <elle/idiom/Open.hh>

namespace nucleus
{
  namespace neutron
  {

    /*---------------.
    | Static Methods |
    `---------------*/

    Record const&
    Record::null()
    {
      static Record record(Record::Type::null);

      return (record);
    }

    /*-------------.
    | Construction |
    `-------------*/

    Record::Record():
      _valid(nullptr)
    {
    }

    Record::Record(Subject const& subject,
                   Permissions permissions,
                   Token const& token):
      _type(Type::valid),
      _valid(new Valid(subject, permissions, token))
    {
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
      _valid(nullptr)
    {
      if (other._valid != nullptr)
        {
          this->_valid =
            new Valid(
              other._valid->subject(),
              other._valid->permissions(),
              other._valid->token() != nullptr ?
              *other._valid->token() :
              Token::null());
        }
    }

    Record::Record(Type const type):
      _type(type),
      _valid(nullptr)
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
    }

    Record::~Record()
    {
      delete this->_valid;
    }

    Record::Valid::Valid():
      _token(nullptr)
    {
    }

    Record::Valid::Valid(Subject const& subject,
                         Permissions permissions,
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
    Record::permissions(Permissions permissions)
    {
      ELLE_ASSERT(this->_type == Type::valid);
      ELLE_ASSERT(this->_valid != nullptr);

      this->_valid->permissions(permissions);
    }

    Token const*
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

      this->_valid->token(token);
    }

    void
    Record::Valid::token(Token const& token)
    {
      delete this->_token;
      this->_token = new Token(token);
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

    elle::Status        Record::Dump(const elle::Natural32 margin) const
    {
      elle::String      alignment(margin, ' ');

      switch (this->_type)
        {
        case Type::null:
          {
            std::cout << alignment << "[Record] " << elle::none << std::endl;

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

            if (this->_valid->token() != nullptr)
              {
                if (this->_valid->token()->Dump(margin + 2) == elle::Status::Error)
                  escape("unable to dump the token");
              }
            else
              std::cout << alignment << elle::io::Dumpable::Shift
                        << "[Record] " << elle::none << std::endl;

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
            stream << "record(null)";
            break;
          }
        case Type::valid:
          {
            ELLE_ASSERT(this->_valid != nullptr);

            stream << "record("
                   << this->_valid->subject()
                   << ", "
                   << this->_valid->permissions()
                   << ")";

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
    Record::symbol()
    {
      ELLE_ASSERT(this->_type == Type::valid);
      ELLE_ASSERT(this->_valid != nullptr);

      return (this->_valid->subject());
    }

  }
}
