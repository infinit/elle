#ifndef ELLE_METRICS_GOOGLE_HH
# define ELLE_METRICS_GOOGLE_HH

#include <string>
#include <cstdint>

#include "../Reporter.hh"

namespace elle
{
  namespace metrics
  {
    namespace google
    {

      class Service: public elle::metrics::Reporter::Service
      {
      public:
        Service(std::string const& host,
                 uint16_t  port,
                 std::string const& tag,
                 std::string const& id);
      private:
        virtual
        void
        _send(elle::metrics::Reporter::TimeMetricPair const& metric) override;

      public:
        virtual
        void
        update_user(std::string const& user) override;
      };

      std::string
      retrieve_id(std::string const& path);

      void
      update_id(std::string const& id);

      // Create a static Reporter with google analytics server.
      void
      service(std::string const& host,
              uint16_t port,
              std::string const& tag,
              std::string const& id);
    }
  }
}

#endif
