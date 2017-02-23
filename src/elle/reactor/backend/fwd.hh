#pragma once

#include <functional>

namespace elle
{
  namespace reactor
  {
    namespace backend
    {
      /// An action run by a thread.
      using Action = std::function<void ()>;
      class Backend;
      class Thread;
    }
  }
}
