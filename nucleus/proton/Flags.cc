#include <nucleus/proton/Flags.hh>
#include <nucleus/Exception.hh>

#include <iostream>
#include <stdexcept>

namespace nucleus
{
  namespace proton
  {
    /*-------.
    | Values |
    `-------*/
    namespace flags
    {
      Flags const none = 0;
      Flags const recursive = (1 << 0);
      Flags const key = (1 << 1);
      Flags const address = (1 << 2);
      Flags const capacity = (1 << 3);
      Flags const footprint = (1 << 4);
      Flags const state = (1 << 5);
      Flags const all =
        recursive | key | address | capacity | footprint | state;
    }

    /*----------.
    | Operators |
    `----------*/

#define NUCLEUS_PROTON_FLAGS_PRINT(_flag_)                              \
  do                                                                    \
    {                                                                   \
      if (empty == false)                                               \
        stream << " | ";                                                \
      stream << #_flag_;                                                \
      empty = false;                                                    \
    } while (0)

    std::ostream&
    operator <<(std::ostream& stream,
                Flags const flags)
    {
      elle::Boolean empty = true;

      if (flags & flags::recursive)
        NUCLEUS_PROTON_FLAGS_PRINT(recursive);
      else if (flags & flags::key)
        NUCLEUS_PROTON_FLAGS_PRINT(key);
      else if (flags & flags::address)
        NUCLEUS_PROTON_FLAGS_PRINT(address);
      else if (flags & flags::capacity)
        NUCLEUS_PROTON_FLAGS_PRINT(capacity);
      else if (flags & flags::footprint)
        NUCLEUS_PROTON_FLAGS_PRINT(footprint);
      else if (flags & flags::state)
        NUCLEUS_PROTON_FLAGS_PRINT(state);
      else
        stream << "none";

      return (stream);
    }
  }
}
