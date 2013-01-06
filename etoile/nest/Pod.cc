#include <etoile/nest/Pod.hh>

#include <nucleus/proton/Egg.hh>

#include <elle/log.hh>

ELLE_LOG_COMPONENT("infinit.etoile.nest.Pod");

namespace etoile
{
  namespace nest
  {
    /*-------------.
    | Construction |
    `-------------*/

    Pod::Pod(nucleus::proton::Egg* egg):
      _state(State::attached),
      _accessed(elle::utility::Time::current()),
      _egg(egg)
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
