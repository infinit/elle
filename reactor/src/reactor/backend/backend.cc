#include <reactor/backend/backend.hh>

namespace reactor
{
  namespace backend
  {
    /*--------.
    | Backend |
    `--------*/

    Backend::~Backend()
    {}

    /*-------------.
    | Construction |
    `-------------*/

    Thread::Thread(const std::string& name,
                   const Action& action):
      _action(action),
      _name(name),
      _status(Status::starting)
    {}

    Thread::~Thread()
    {}

    /*------.
    | State |
    `------*/

    void
    Thread::status(Status status)
    {
      this->_status = status;
    }
  }
}
