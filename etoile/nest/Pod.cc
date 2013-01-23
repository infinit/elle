#include <etoile/nest/Pod.hh>

#include <nucleus/proton/Egg.hh>

namespace etoile
{
  namespace nest
  {
    /*-------------.
    | Construction |
    `-------------*/

    Pod::Pod(std::shared_ptr<nucleus::proton::Egg>& egg):
      _state(State::attached),
      _accessed(elle::utility::Time::current()),
      _egg(egg)
    {
    }

    Pod::Pod(std::shared_ptr<nucleus::proton::Egg>&& egg):
      _state(State::attached),
      _accessed(elle::utility::Time::current()),
      _egg(std::move(egg))
    {
    }

    /*----------.
    | Printable |
    `----------*/

    void
    Pod::print(std::ostream& stream) const
    {
      ELLE_ASSERT(this->_egg != nullptr);

      stream << this->_state
             << "(" << this->_accessed << ", " << *this->_egg << ")";
    }

    /*----------.
    | Operators |
    `----------*/

    std::ostream&
    operator <<(std::ostream& stream,
                Pod::State const state)
    {
      switch (state)
        {
        case Pod::State::attached:
          {
            stream << "attached";
            break;
          }
        case Pod::State::detached:
          {
            stream << "detached";
            break;
          }
        default:
          {
            throw elle::Exception("unknown state: '%s'",
                                  static_cast<int>(state));
          }
        }

      return (stream);
    }
  }
}
