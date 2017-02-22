#pragma once

#if !defined(INFINIT_MACOSX) && !defined(INFINIT_IOS)
# error "Unsupported platform"
#endif

#include <string>

#include <boost/signals2.hpp>

#include <elle/attribute.hh>

namespace elle
{
  namespace reactor
  {
    namespace network
    {
      class Reachability
      {
      public:
        enum class NetworkStatus
        {
          Unreachable   = 0,
          Reachable     = 1,
# ifdef INFINIT_IOS
          ReachableWWAN = 2, // Reachable using mobile data connection.
# endif
        };

        typedef void (StatusCallback) (NetworkStatus);

      public:
        /// Leaving the host empty tests zero address.
        Reachability(boost::optional<std::string> host = {},
                     std::function<StatusCallback> const& callback = {},
                     bool start = false);
        ~Reachability();

        void
        start();
        void
        stop();

      public:
        bool
        reachable() const;
        NetworkStatus
        status() const;
        std::string
        status_string() const;

        ELLE_ATTRIBUTE_X(boost::signals2::signal<StatusCallback>, status_changed);

        class Impl;

      private:
        ELLE_ATTRIBUTE(Impl*, impl);
      };
    }
  }
}
