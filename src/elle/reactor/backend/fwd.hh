#pragma once

#include <functional>

namespace elle
{
  namespace reactor
  {
    namespace backend
    {
      /// An action run by a thread.
      typedef std::function<void ()> Action;
      class Backend;
      class Thread;
    }
  }
}
