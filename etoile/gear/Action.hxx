#ifndef GEAR_ACTION_HXX
# define GEAR_ACTION_HXX

namespace etoile
{
  namespace gear
  {
//
// ---------- action ----------------------------------------------------------
//

    /*--------.
    | Methods |
    `--------*/

    template <typename T>
    void
    Action::apply() const
    {
      switch (this->_type)
        {
        case Type::push:
          {
            ELLE_ASSERT(dynamic_cast<action::Push const*>(this) != nullptr);
            action::Push const* action = static_cast<action::Push const*>(this);

            // XXX[to change by calling a method on an object]
            T::Push(action->address(), action->block());

            break;
          }
        case Type::wipe:
          {
            ELLE_ASSERT(dynamic_cast<action::Wipe const*>(this) != nullptr);
            action::Wipe const* action = static_cast<action::Wipe const*>(this);

            // XXX[to change by calling a method on an object]
            T::Wipe(action->address());

            break;
          }
        }
    }
  }
}

#endif
