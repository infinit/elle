#include <nucleus/proton/Handle.hh>
#include <nucleus/proton/Porcupine.hh>

#include <cryptography/SecretKey.hh>

namespace nucleus
{
  namespace proton
  {
//
// ---------- Handle ----------------------------------------------------------
//

    /*-------------.
    | Construction |
    `-------------*/

    Handle::Handle():
      _state(State::unnested),
      _identity(nullptr)
    {
      // Manually set all the union pointers to null so as to make sure all
      // the cases are handled.
      this->_identity = nullptr;
      this->_egg = nullptr;
    }

    Handle::Handle(Address const& address,
                   cryptography::SecretKey const& secret):
      _state(State::unnested),
      _identity(new Identity{address, secret})
    {
    }

    Handle::Handle(std::shared_ptr<Egg>& egg):
      _state(State::nested),
      _egg(egg)
    {
    }

    Handle::Handle(Handle const& other):
      _state(other._state)
    {
      // Manually set all the union pointers to null so as to make sure all
      // the cases are handled.
      this->_identity = nullptr;
      this->_egg = nullptr;

      switch (this->_state)
        {
        case State::unnested:
          {
            ELLE_ASSERT(other._identity != nullptr);

            this->_identity = new Identity{*other._identity};

            break;
          }
        case State::nested:
          {
            ELLE_ASSERT(other._egg != nullptr);

            this->_egg = other._egg;

            break;
          }
        default:
          throw Exception("unknown state '%s'", this->_state);
        }
    }

    Handle::~Handle()
    {
      switch (this->_state)
        {
        case State::unnested:
          {
            delete this->_identity;

            break;
          }
        case State::nested:
          {
            break;
          }
        default:
          throw Exception("unknown state '%s'", this->_state);
        }
    }

    /*--------.
    | Methods |
    `--------*/

    Address const&
    Handle::address() const
    {
      switch (this->_state)
        {
        case State::unnested:
          {
            ELLE_ASSERT(this->_identity != nullptr);

            return (this->_identity->address());
          }
        case State::nested:
          {
            ELLE_ASSERT(this->_egg != nullptr);

            return (this->_egg->address());
          }
        default:
          throw Exception("unknown state '%s'", this->_state);
        }
    }

    cryptography::SecretKey const&
    Handle::secret() const
    {
      switch (this->_state)
        {
        case State::unnested:
          {
            ELLE_ASSERT(this->_identity != nullptr);

            return (this->_identity->secret());
          }
        case State::nested:
          {
            ELLE_ASSERT(this->_egg != nullptr);

            return (this->_egg->secret());
          }
        default:
          throw Exception("unknown state '%s'", this->_state);
        }
    }

    std::shared_ptr<Egg> const&
    Handle::egg() const
    {
      ELLE_ASSERT(this->_state == State::nested);
      ELLE_ASSERT(this->_egg != nullptr);

      return (this->_egg);
    }

    std::shared_ptr<Egg>&
    Handle::egg()
    {
      ELLE_ASSERT(this->_state == State::nested);
      ELLE_ASSERT(this->_egg != nullptr);

      return (this->_egg);
    }

    void
    Handle::place(std::shared_ptr<Egg>& egg)
    {
      ELLE_ASSERT(this->_state != State::nested);

      // Delete the previous identity structure.
      ELLE_ASSERT(this->_identity != nullptr);
      delete this->_identity;
      this->_identity = nullptr;

      // Set the egg to reference to access the block.
      this->_egg = egg;

      // Update the state.
      this->_state = State::nested;
    }

    void
    Handle::reset(Address const& address,
                  cryptography::SecretKey const& secret)
    {
      switch (this->_state)
        {
        case State::unnested:
          {
            ELLE_ASSERT(this->_identity != nullptr);

            // In this case, regenerate the identity.
            delete this->_identity;
            this->_identity = nullptr;
            this->_identity = new Identity{address, secret};

            break;
          }
        case State::nested:
          {
            ELLE_ASSERT(this->_egg != nullptr);

            // In this case, reset the egg.
            this->_egg->reset(address, secret);

            break;
          }
        default:
          throw Exception("unknown state '%s'", this->_state);
        }
    }

    /*----------.
    | Operators |
    `----------*/

    elle::Boolean
    Handle::operator ==(Handle const& other) const
    {
      if (this == &other)
        return true;

      // Compare the handles depending on their state.
      if ((this->_state == State::nested) &&
          (other._state == State::nested))
        {
          // In this case, both handles reference a transient block through
          // an egg whose memory address can be compared to know if both track
          // the same block.

          return (this->_egg.get() == other._egg.get());
        }
      else
        {
          // Otherwise, one of the two handles represent a block which
          // has not yet been retrieved from the network. This block is
          // therefore permanent i.e has an address.
          //
          // Since transient blocks have a temporary address (until they get
          // bound), it would not be wise to base the comparison on it
          // as, if being extremely unlucky, the temporary address could
          // end up being the same as another permanent block.
          //
          // The following therefore considers that if one of the block is
          // transient (this other cannot be otherwise the 'if' case would
          // have been true), then the handles reference different blocks.
          //
          // Otherwise, both blocks are permanent in which case their addresses
          // can be compared, no matter the state of the handle.

          // Check whether one of the handles reference a nested and transient
          // block.
          if (((this->_state == State::nested) &&
               (this->_egg->type() == Egg::Type::transient)) ||
              ((other._state == State::nested) &&
               (other._egg->type() == Egg::Type::transient)))
            return (false);

          // At this point we know that both blocks are permanent being unnested
          // or nested; their addresses can therefore be compared.
          return (this->address() == other.address());
        }

      elle::unreachable();
    }

    /*---------.
    | Dumpable |
    `---------*/

    elle::Status
    Handle::Dump(const elle::Natural32      margin) const
    {
      elle::String alignment(margin, ' ');

      std::cout << alignment << "[Handle] " << this->_state << std::endl;

      switch (this->_state)
        {
        case State::unnested:
          {
            ELLE_ASSERT(this->_identity != nullptr);

            std::cout << alignment << elle::io::Dumpable::Shift
                      << "[Identity] " << *this->_identity << std::endl;

            break;
          }
        case State::nested:
          {
            ELLE_ASSERT(this->_egg != nullptr);

            std::cout << alignment << elle::io::Dumpable::Shift
                      << "[Egg] " << *this->_egg << std::endl;

            break;
          }
        default:
          throw Exception("unknown state '%s'", this->_state);
        }

      return elle::Status::Ok;
    }

    /*----------.
    | Printable |
    `----------*/

    void
    Handle::print(std::ostream& stream) const
    {
      stream << this->_state << "(";

      switch (this->_state)
        {
        case State::unnested:
          {
            ELLE_ASSERT(this->_identity != nullptr);

            stream << *this->_identity;

            break;
          }
        case State::nested:
          {
            ELLE_ASSERT(this->_egg != nullptr);

            stream << *this->_egg;

            break;
          }
        default:
          throw Exception("unknown state '%s'", this->_state);
        }

      stream << ")";
    }

    /*----------.
    | Operators |
    `----------*/

    std::ostream&
    operator <<(std::ostream& stream,
                Handle::State const state)
    {
      switch (state)
        {
        case Handle::State::unnested:
          {
            stream << "unnested";
            break;
          }
        case Handle::State::nested:
          {
            stream << "nested";
            break;
          }
        default:
          {
            throw Exception("unknown state: '%s'", static_cast<int>(state));
          }
        }

      return (stream);
    }

//
// ---------- Identity --------------------------------------------------------
//

    /*-------------.
    | Construction |
    `-------------*/

    Handle::Identity::Identity(Address const& address,
                               cryptography::SecretKey const& secret):
      _address(address),
      _secret(secret)
    {
    }

    Handle::Identity::Identity(Identity const& other):
      _address(other._address),
      _secret(other._secret)
    {
    }

    /*----------.
    | Printable |
    `----------*/

    void
    Handle::Identity::print(std::ostream& stream) const
    {
      stream << this->_address
             << ", "
             << this->_secret;
    }
  }
}
