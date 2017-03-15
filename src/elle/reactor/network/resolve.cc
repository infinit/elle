#include <elle/reactor/network/resolve.hh>

#include <utility>

#include <elle/log.hh>
#include <elle/printf.hh>

#include <elle/reactor/network/exception.hh>
#include <elle/reactor/Operation.hh>
#include <elle/reactor/scheduler.hh>

ELLE_LOG_COMPONENT("elle.reactor.network.resolve");

namespace elle
{
  namespace reactor
  {
    namespace network
    {
      namespace
      {
        template <typename Protocol>
        class Resolution: public Operation
        {
        public:
          using Super = Operation;
          using Resolver = typename Protocol::resolver;
          using EndPoint = typename Protocol::resolver::endpoint_type;
          Resolution(std::string  hostname, std::string  service,
                     bool ipv4_only)
            : Super{*reactor::Scheduler::scheduler()}
            , _resolver{reactor::Scheduler::scheduler()->io_service()}
            , _canceled{false}
            , _hostname{std::move(hostname)}
            , _service{std::move(service)}
            , _end_point{}
            , _ipv4_only{ipv4_only}
          {}

          void
          print(std::ostream& stream) const override
          {
            elle::fprintf(stream,
                          "resolution of %s: %s", this->_hostname, this->_service);
          }

        protected:
          void
          _abort() override
          {
            ELLE_TRACE_SCOPE("%s: abort", *this);
            this->_canceled = true;
            this->_resolver.cancel();
            reactor::wait(*this);
          }

          void
          _start() override
          {
            ELLE_TRACE("resolve %s:%s", this->_hostname, this->_service);
            using Query = typename Resolver::query;
            auto query =
              this->ipv4_only()
              ? Query(Protocol::v4(), this->_hostname, this->_service)
              : Query(this->_hostname, this->_service);
            this->_resolver.async_resolve(
              query,
              [this](const boost::system::error_code& error,
                     typename Resolver::iterator it)
              {
                this->_wakeup(error, it);
              });
          }

        private:
          void
          _wakeup(const boost::system::error_code& error,
                  typename Resolver::iterator it)
          {
            if (this->_canceled)
              ELLE_TRACE_SCOPE("%s: canceled", *this);
            else if (error)
            {
              ELLE_TRACE_SCOPE("%s: ended with error: %s", *this, error.message());
              this->_raise<ResolutionError>(this->_hostname, error.message());
            }
            else if (it == typename Resolver::iterator::basic_resolver_iterator())
            {
              // From the boost documentation:
              //   A successful resolve operation is guaranteed to pass at least
              //   one entry to the handler.
              // This assumption is false on wine.
              ELLE_TRACE_SCOPE(
                "%s: ended with no error but an empty address list", *this);
              this->_raise<ResolutionError>(
                this->_hostname, "host not found: address list is empty");
            }
            else
            {
              ELLE_TRACE_SCOPE("%s: ended", *this);
              this->_end_point = *it;
              ELLE_DEBUG("%s: endpoint address: %s",
                         *this, this->_end_point.address().to_string());
            }
            this->done();
          }

          ELLE_ATTRIBUTE(Resolver, resolver);
          ELLE_ATTRIBUTE(bool, canceled);
          ELLE_ATTRIBUTE(std::string, hostname);
          ELLE_ATTRIBUTE(std::string, service);
          ELLE_ATTRIBUTE_R(EndPoint, end_point);
          ELLE_ATTRIBUTE_R(bool, ipv4_only);
        };

        template <typename Protocol>
        typename Protocol::resolver::endpoint_type
        resolve(const std::string& hostname,
                const std::string& service,
                bool ipv4_only)
        {
          Resolution<Protocol> resolution(hostname, service, ipv4_only);
          resolution.run();
          return resolution.end_point();
        }
      }

      boost::asio::ip::tcp::endpoint
      resolve_tcp(const std::string& hostname,
                  const std::string& service,
                  bool ipv4_only)
      {
        return resolve<boost::asio::ip::tcp>(hostname, service, ipv4_only);
      }

      boost::asio::ip::udp::endpoint
      resolve_udp(const std::string& hostname,
                  const std::string& service,
                  bool ipv4_only)
      {
        return resolve<boost::asio::ip::udp>(hostname, service, ipv4_only);
      }
    }
  }
}
